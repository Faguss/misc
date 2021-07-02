// Lockmouse2 v1.16 by Faguss (ofp-faguss.com) 02.07.21

    // Headers
#include <windows.h>       // winapi
#include <iostream>        // cout
#include <sstream>         // var conversion
#include <tlhelp32.h>      // process/module traversing
#include <vector>
#include <algorithm>       // tolower

	// SystemParametersInfo parameters
#define SPI_GETMOUSESPEED 0x0070
#define SPI_SETMOUSESPEED 0x0071

	// Namespaces
using namespace std;





// **************** FUNCTIONS **************************************************

	// Check if current window is not Windows Explorer
bool IsExplorer(DWORD pid)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
 
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 xModule;
		xModule.dwSize = sizeof(MODULEENTRY32);
		bool result    = false;
		
		if (Module32First(hSnap, &xModule) != 0)
			if (lstrcmpi(xModule.szModule, (LPCTSTR)"explorer.exe") == 0)
				result = true;
		
		CloseHandle(hSnap);
		return result;
	} else
		return false;

}



	// Check if process with given ID number exists
bool IsProcessRunning(DWORD pid)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize       = sizeof(processInfo);
	bool result              = false;
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (processesSnapshot != INVALID_HANDLE_VALUE) {
		Process32First(processesSnapshot, &processInfo);
		
		do {
			result = (DWORD)processInfo.th32ProcessID == pid;
		} while (Process32Next(processesSnapshot, &processInfo)  &&  !result);

		CloseHandle(processesSnapshot);
	}
	
	return result;
}



	// Find process by exename; returns PID number
DWORD findProcess(const char* name)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize       = sizeof(processInfo);
	DWORD pid_return         = 0;
	DWORD pid_current        = GetCurrentProcessId();
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (processesSnapshot != INVALID_HANDLE_VALUE) {
		Process32First(processesSnapshot, &processInfo);
		
		do {
			DWORD pid = processInfo.th32ProcessID;
			
			if (strcmpi(processInfo.szExeFile,name)==0  &&  pid!=pid_current) {
				pid_return = pid;
				break;
			}
		} while (Process32Next(processesSnapshot, &processInfo));

		CloseHandle(processesSnapshot);
	}
	
	return pid_return;
};



	// Change OFP sensitivity in process memory
bool slowDownMouse(DWORD pid, string WINDOW_NAME, float sensitivity)
{
	int offset = 0;
	
	if (WINDOW_NAME == "operation flashpoint") 
		offset = 0x0079E938;
	else
		if (WINDOW_NAME == "cold war assault") 
			offset = 0x0078DA30;
		else
			return false;
		
	SIZE_T stBytes = 0;
	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	
	if (!phandle) 
		return false;
	
	WriteProcessMemory(phandle, (LPVOID)offset,     &sensitivity, 4, &stBytes);
	WriteProcessMemory(phandle, (LPVOID)(offset+4), &sensitivity, 4, &stBytes);
	//cout << "changed sensitivity\n";
	
	CloseHandle(phandle);
	return true;
};



// Return number of threads in a process
// http://msdn.microsoft.com/en-us/library/ms686852%28VS.85%29.aspx
int countThreads(DWORD pid)
{
	int threads        = 0;
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	hThreadSnap        = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	
	if (hThreadSnap != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te32; 
		te32.dwSize = sizeof(THREADENTRY32);
		
		if (Thread32First(hThreadSnap, &te32)) {
			do {
				if (te32.th32OwnerProcessID == pid) 
					threads++;
			} while (Thread32Next(hThreadSnap, &te32));
		}

		CloseHandle(hThreadSnap);
	}
	
    return threads; 
}
// *****************************************************************************





















// **************** MAIN PROGRAM ***********************************************

int main(int argc, char *argv[])
{
	SetConsoleTitle( "Lockmouse2" );
	
    cout << "\n\tLockmouse2 v1.16\n\n";
    
// arguments:
// -window=
// -center
// -x=
// -y=
// -nosetcursor
// -sensitivity=
// -acceleration=
// -OFPsensitivity=
// -quit
// -minimize


	// Arguments variables
	float ofp_sensitivity        = 1;
	bool ofp_sensitivity_change  = false;
	bool reposition_center       = false;
	bool reposition_cursor       = true;
	bool reposition_x            = false;
	bool reposition_y            = false;
	bool quit                    = false;		 
	int acceleration_original[3] = {-1,-1,-1};
	int acceleration[3]          = {6,10,-1};
	int sensitivity_original     = -1;
	int sensitivity              = 10;
	int coordinate_x             = 0;
	int coordinate_y             = 0;
		
	vector<string> window_names;
	window_names.push_back("operation flashpoint");
	window_names.push_back("cold war assault");
	window_names.push_back("arma resistance");
	
		

    
	// Parse arguments
	for (int i=1; i<argc; i++) {  
		string argument = string(argv[i]);
		transform(argument.begin(), argument.end(), argument.begin(), ::tolower);

		// Custom window name
		if (argument.substr(0,8)=="-window="  &&  argument.length()>8) {
			string wanted_window = argument.substr(8);
			window_names.push_back(wanted_window);
		}
   
		// X coordinate
		if (argument.substr(0,3)=="-x="  &&  argument.length()>3) {
			reposition_x = true;
			istringstream iss(argument.substr(3));
			iss >> coordinate_x;
		}

		// Y coordinate
		if (argument.substr(0,3)=="-y="  &&  argument.length()>3) {
			reposition_y = true;
			istringstream iss(argument.substr(3));
			iss >> coordinate_y;
		}

		// Windows mouse sensitivity
		if (argument.substr(0,13)=="-sensitivity="  &&  argument.length()>13) {
			istringstream iss(argument.substr(13));
			iss >> sensitivity;
			
			if (sensitivity < 1) 
				sensitivity = 1;
				
			if (sensitivity > 20) 
				sensitivity = 20;
				
			SystemParametersInfo(SPI_GETMOUSESPEED, 0, &sensitivity_original, 0);
		}

		// Windows mouse acceleration (it's called 'precision' in main.cpl)
		if (argument.substr(0,14)=="-acceleration="  &&  argument.length() > 14) {
			istringstream iss(argument.substr(14));
			iss >> acceleration[2];
			
			if (acceleration[2]!=0  &&  acceleration[2]!=1) 
				acceleration[2] = -1;
				
			SystemParametersInfo(SPI_GETMOUSE, 0, &acceleration_original, 0);
			
			// This requires array with 3 numbers
			// Last number is option on/off
			// Set first two numbers only if wanted setting is different than current
			if (acceleration[2] >= 0  &&  acceleration_original[2] != acceleration[2]) {
				if (acceleration_original[2] == 0) {
					//cout << "acceleration is disabled, user wants to enable it\n",
					acceleration[0] = 6,
					acceleration[1] = 10;
				}
       
				if (acceleration_original[2] == 1) {
					//cout << "acceleration is enabled, user wants to disable it\n",
					acceleration[0] = 0,
					acceleration[1] = 0;
				}
			} else 
				acceleration[2] = -1;
		}

		// OFP cursor sensitivity
		if (argument.substr(0,16)=="-ofpsensitivity="  &&  argument.length() > 16) {
			istringstream iss(argument.substr(16));
			iss >> ofp_sensitivity;
			ofp_sensitivity_change = true;
		}

		// Center window on the screen
		if (argument == "-center") 
			reposition_center = true;
		
		// Don't change cursor position
		if (argument == "-nosetcursor") 
			reposition_cursor = false;
		
		// Quit lockmouse on game exit
		if (argument == "-quit") 
			quit = true;
		
		// Minimize window right now
		if (argument == "-minimize") {
			HWND lockwindow = FindWindow(NULL, "Lockmouse2");
			
			if (lockwindow) 
				ShowWindow(lockwindow, SW_MINIMIZE);
        }
	}


	// Display enabled parameters
	cout << "Window: ";
	for (int i=0; i<window_names.size(); i++) {
		if (i !=0) cout << ", ";
		cout << window_names[i];
	}
	cout << endl;

	if (reposition_center)
		cout << "Center window\n";
	else {
		if (reposition_x) 
			cout << "X: " << coordinate_x << endl;
			
		if (reposition_y) 
			cout << "Y: " << coordinate_y << endl;
	}

	if (!reposition_cursor) 
		cout << "Keep cursor\n";
		
	if (sensitivity_original > 0) 
		cout << "Windows sensitivity: " << sensitivity << endl;
		
	if (acceleration[2] >= 0) 
		cout << "Windows acceleration: " << acceleration[2] << endl;
		
	if (ofp_sensitivity_change) 
		cout << "OFP Sensitivity: " << ofp_sensitivity << endl;
		
	if (quit) 
		cout << "Quit with application\n";

	cout << "\nHold CTRL+A to release. CTRL+C in the console to quit.\n\n";



	
	
	
	



	
	
	
	
	
	
	// Define variables
	HWND hwnd_current = NULL;
	HWND hwnd         = NULL;
	string current_window;
	POINT pt;
	POINT pt_old;
	RECT nc;
	RECT game;
	RECT game_client;
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);

	int border_left      = 0;
	int border_right     = 0;
	int border_bottom    = 0;
	int border_top       = 0;
	bool mouse_click     = true;
	bool relaunched_game = true;
	bool window_match    = false;
	
	// Control variables for parameters
	bool check_cursor             = true;
	int changed_acceleration      = -1;
	int changed_sensitivity       = -1;
	DWORD pid_changed_sensitivity = 0;
	DWORD pid_current             = 0;
	DWORD pid_last_working        = 0;
	
	if (sensitivity > 0) 
		changed_sensitivity = 0;

	if (acceleration[2] >= 0) 
		changed_acceleration = 0;
    
	





	// Infinite loop until CTRL + C is pressed
    while (true) {
		Sleep(20);

		// Get active window
		hwnd_current = GetForegroundWindow();
		
		if (hwnd_current != NULL  &&  hwnd_current != hwnd) {			// for a new window
		    window_match = false;
            hwnd         = hwnd_current;
            char window_char[1024];
            
			GetWindowText(hwnd, window_char, 1024),
			GetWindowThreadProcessId(hwnd, &pid_current),
			current_window = string(window_char),
			transform(current_window.begin(), current_window.end(), current_window.begin(), ::tolower);
			
			for (int i=0; i<window_names.size() && !window_match; i++)
				if (current_window == window_names[i])
					window_match = true;
		}






		// If active window isn't what we wanted OR if it's explorer ---------------------------------------
		if (!window_match  ||  IsExplorer(pid_current)) {
			// Unlock cursor and reset click var
			ClipCursor(NULL);
			mouse_click = true;
			
			// Bring back cursor to the original position
			if (reposition_cursor && !check_cursor) {
				check_cursor = true;
				SetCursorPos(pt_old.x, pt_old.y);
			}
			
			// Bring back original mouse sensitivity
			if (changed_sensitivity == 1 && SystemParametersInfo(SPI_SETMOUSESPEED, 0, (void*)sensitivity_original, 0) == 1) {
				changed_sensitivity = 0;
				//cout<<"brought back sens\n";
			}
			
			// Bring back original mouse acceleration
			if (changed_acceleration == 1 && SystemParametersInfo(SPI_SETMOUSE, 0, (void*)acceleration_original, 0) == 1) {
				changed_acceleration = 0; 
				//cout<<"brought back acc\n";
			}
            
			// Check if application still exists
			if (pid_last_working != 0  &&  !IsProcessRunning(pid_last_working)) {
				relaunched_game = true;
				
				// if user passed -quit
				if (quit) {
					// If game is flashpoint
					if (current_window=="operation flashpoint"  ||  current_window=="cold war assault"  ||  current_window=="arma resistance") {
						// if gameRestart is not running then quit
						if (findProcess("gameRestart.exe") == 0) 
							return 1; 
						else {
							pid_last_working = 0;
							Sleep(1000);
						}
					} else {
						// If it's not flashpoint then just quit
						//cout << "app exit";
						return 1;
					}
				}
			}
            
			continue;
		}
		// -------------------------------------------------------------------------------------------------








		// Save process ID
		pid_last_working = pid_current;
		
		
		// Change windows mouse sensitivity -------------------------------------------
		if (changed_sensitivity == 0  &&  SystemParametersInfo(SPI_SETMOUSESPEED, 0, (void*)sensitivity, 0) == 1) {
			changed_sensitivity = 1; 
			//cout << "changed sens\n";
		}
		
		// Change mouse acceleration --------------------------------------------------
		if (changed_acceleration == 0  &&  SystemParametersInfo(SPI_SETMOUSE, 0, (void*)acceleration, 0) == 1) {
			changed_acceleration = 1; 
			//cout<<"changed acc\n";
		}
		
		// Change mouse speed in OFP --------------------------------------------------
		if (ofp_sensitivity_change  &&  pid_current != pid_changed_sensitivity  &&  slowDownMouse(pid_current, current_window, ofp_sensitivity)) {
			pid_changed_sensitivity = pid_current;
		}


		// Change window position -----------------------------------------------------
		if (relaunched_game) {
			relaunched_game = false;
			GetWindowRect(hwnd, &game);
			GetClientRect(hwnd, &game_client);

			// Calculate window border size
			border_left   = abs((game.right - game.left) - game_client.right) / 2;
			border_right  = border_left;
			border_bottom = border_left;
			border_top    = abs((game.bottom - game.top) - game_client.bottom) - border_left;
			
			/*cout << "border_left: " <<  border_left << endl;
			cout << "border_right: " <<  border_right << endl;
			cout << "border_bottom: " <<  border_bottom << endl;
			cout << "border_top: " <<  border_top << endl;*/

			if (reposition_center || reposition_x || reposition_y) {
				// Reset coordinates if not set
				if (!reposition_x) 
					coordinate_x = -border_left;
					
				if (!reposition_y) 
					coordinate_y = -border_top;
				
				// Calculate coordinates to center window
				if (reposition_center) {
					// client.right is window width and client.bottom is window height
					coordinate_x = ((desktop.right - game_client.right) / 2) - border_left;
					coordinate_y = ((desktop.bottom - game_client.bottom) / 2) - border_top;
					
					/*cout << "coordinate_x: " << coordinate_x << endl;
					cout << "coordinate_y: " << coordinate_y << endl << endl;*/
				}
				
				SetWindowPos(hwnd, 0, coordinate_x, coordinate_y, 0, 0, SWP_NOSIZE);
				Sleep(10);

				/*GetWindowRect(hwnd, &game);
				GetClientRect(hwnd, &gameCLIENT);
				cout << "game.left: " << game.left << endl;
				cout << "game.right: " << game.right << endl;
				cout << "game.top: " << game.top << endl;
				cout << "game.bottom: " << game.bottom << endl;
				cout << "game_client.left: " << game_client.left << endl;
				cout << "game_client.right: " << game_client.right << endl;
				cout << "game_client.top: " << game_client.top << endl;
				cout << "game_client.bottom: " << game_client.bottom << endl;*/
				
				continue;
			}
		}


		// Change cursor position -----------------------------------------------------
		if (reposition_cursor && check_cursor) {
			GetCursorPos(&pt_old);
			GetWindowRect(hwnd, &game);
			
			// For some reason the window is repositioned if you activate it by clicking on the border
			// this condition prevents it
            if 
			(
				// If inside game window (not on borders)
				pt_old.x > game.left+border_left && 
				pt_old.x < game.right-border_right &&
				pt_old.y > game.top+border_top &&
				pt_old.y < game.bottom-border_bottom
				||
				// OR outside game window
				pt_old.x < game.left ||
				pt_old.x > game.right ||
				pt_old.y < game.top ||
				pt_old.y > game.bottom
            )
				// then set mouse cursor in the lower right corner			
				SetCursorPos(game.right-border_right-1, game.bottom-border_bottom-1);

			check_cursor = false;
		}

		



		
		




		// If CTRL+A isn't pressed
		if (GetAsyncKeyState(VK_CONTROL) >= 0  ||  GetAsyncKeyState('A') >= 0) {
			GetWindowRect(hwnd, &game);	
			GetCursorPos(&pt);
			
			bool is_ofp = current_window=="operation flashpoint"  ||  current_window=="cold war assault" || current_window=="arma resistance";
            
			// If cursor is in game window
            if 
			(
				pt.x > game.left+border_left && 
				pt.x < game.right-border_right &&
				pt.y > game.top+border_top &&
				pt.y < game.bottom-border_bottom &&
				(!is_ofp  ||  is_ofp && countThreads(pid_current)>=5)
				// If it's not OFP  or OFP and has at least 5 threads
            )
            {
				// Then block it
				nc.top    = pt.y;
				nc.left   = pt.x;
				nc.right  = pt.x+1;
				nc.bottom = pt.y+1;
				ClipCursor(&nc);
				
				// Simulate a single mouse click to activate that window
				if (mouse_click) {
					mouse_click = false;
					SetCapture(hwnd);
					PostMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
					Sleep(5);
					PostMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
					ReleaseCapture();
				}
            }
		} else {
			// Otherwise release cursor
			ClipCursor(NULL);
			GetCursorPos(&pt_old);
		}
    }
    
	return 0;
}


