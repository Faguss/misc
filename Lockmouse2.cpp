// Lockmouse2 v1.15 by Faguss (ofp-faguss.com) 09.01.15

    // Headers
#include <windows.h>       // winapi
#include <iostream>        // cout
#include <sstream>         // var conversion
#include <tlhelp32.h>      // process/module traversing

	// SystemParametersInfo parameters
#define SPI_GETMOUSESPEED 0x0070
#define SPI_SETMOUSESPEED 0x0071

	// Namespaces
using namespace std;





// **************** FUNCTIONS **************************************************

	// Check if current window is not Windows Explorer
bool IsNotExplorer (DWORD pid)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 xModule;
	bool result = false;
 
	if (hSnap != INVALID_HANDLE_VALUE) 
	{
		xModule.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnap, &xModule) != 0)
			if (lstrcmpi(xModule.szModule, (LPCTSTR)"explorer.exe") != 0)
				result = true;
		
		CloseHandle(hSnap);
		return result;
	}
	else
		return false;

};



	// Check if process with given ID number exists
bool IsProcessRunning (DWORD pid)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	bool result = false;

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ( processesSnapshot != INVALID_HANDLE_VALUE )
	{
		Process32First(processesSnapshot, &processInfo);
		
		do
            if ((DWORD)processInfo.th32ProcessID == pid) 
				result = true;
		while (Process32Next(processesSnapshot, &processInfo)  &&  !result);

		CloseHandle(processesSnapshot);
	};
	
	return result;
};



	// Find process by exename; returns PID number
int findProcess (char* name)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	int pid = 0,
		currPID = GetCurrentProcessId();

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (processesSnapshot != INVALID_HANDLE_VALUE)
	{
		Process32First(processesSnapshot, &processInfo);
		
		do
		{
			int tempPID = processInfo.th32ProcessID;
			if (strcmpi(processInfo.szExeFile,name)==0  &&  tempPID!=currPID)
			{
				pid = tempPID;
				break;
			};
		}
		while (Process32Next(processesSnapshot, &processInfo));

		CloseHandle(processesSnapshot);
	};
	
	return pid;
};



	// Change OFP sensitivity in process memory
bool slowDownMouse (DWORD pid, string WINDOW_NAME, float sensitivity)
{
	SIZE_T stBytes=0;
	HANDLE phandle;
	int offset=0;
	
	if (WINDOW_NAME=="operation flashpoint") 
		offset=0x0079E938;
	else
		if (WINDOW_NAME=="cold war assault") 
			offset=0x0078DA30;
		else
			return false;
		
	phandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	if (phandle == 0) 
		return false;
	
	WriteProcessMemory(phandle, (LPVOID)offset,     &sensitivity, 4, &stBytes);
	WriteProcessMemory(phandle, (LPVOID)(offset+4), &sensitivity, 4, &stBytes);
	//cout << "changed sensitivity\n";
	
	CloseHandle(phandle);
	return true;
};



// Return number of threads in a process
// http://msdn.microsoft.com/en-us/library/ms686852%28VS.85%29.aspx
int countThreads (unsigned int pid)
{
	int threads = 0;
	THREADENTRY32 te32; 
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
 
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if (hThreadSnap != INVALID_HANDLE_VALUE) 
	{
		te32.dwSize = sizeof(THREADENTRY32);
		if (Thread32First( hThreadSnap, &te32 )) 
		{
			do 
				if (te32.th32OwnerProcessID == pid) 
					threads++;
			while (Thread32Next(hThreadSnap, &te32));
		};

		CloseHandle( hThreadSnap );
	};
	
    return threads; 
};
// *****************************************************************************





















// **************** MAIN PROGRAM ***********************************************

int main(int argc, char *argv[])
{
	SetConsoleTitle( "Lockmouse2" );
	
    cout << "\n\tLockmouse2 v1.15\n\n";
    
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
	string WANTED_WINDOW = "operation flashpoint";
	float OFPsensitivity = 1;
	
	bool changeOFPsensitivity = false,
		 repositionCENTER = false,
		 repositionCURSOR = true,
		 repositionX = false,
		 repositionY = false,
		 quit = false;
		 
	int ORIGINALacceleration[3] = {-1,-1,-1},
		acceleration[3] = {6,10,-1},
		ORIGINALsensitivity = -1,
		sensitivity = 10,
		Xcoord = 0, 
		Ycoord = 0;
		

    
	// Parse arguments
	for (int i=1; i<argc; i++)
	{  
		string currARG = string(argv[i]);

		// Custom window name
		if(currARG.substr(0,8)=="-window="  &&  currARG.length()>8)
		{
			WANTED_WINDOW = currARG.substr(8);
			transform(WANTED_WINDOW.begin(), WANTED_WINDOW.end(), WANTED_WINDOW.begin(), ::tolower);
		};
   
		// X coordinate
		if(currARG.substr(0,3)=="-x="  &&  currARG.length()>3)
		{
			repositionX = true;
			istringstream iss(currARG.substr(3));
			iss >> Xcoord;
		};

		// Y coordinate
		if(currARG.substr(0,3)=="-y="  &&  currARG.length()>3)
		{
			repositionY = true;
			istringstream iss(currARG.substr(3));
			iss >> Ycoord;
		};

		// Windows mouse sensitivity
		if(currARG.substr(0,13)=="-sensitivity="  &&  currARG.length()>13)
		{
			istringstream iss(currARG.substr(13));
			iss >> sensitivity;
			if (sensitivity < 1) sensitivity=1;
			if (sensitivity > 20) sensitivity=20;
			SystemParametersInfo(SPI_GETMOUSESPEED, 0, &ORIGINALsensitivity, 0);
		};

		// Windows mouse acceleration (it's called 'precision' in main.cpl)
		if(currARG.substr(0,14)=="-acceleration="  &&  currARG.length() > 14)
		{
			istringstream iss(currARG.substr(14));
			iss >> acceleration[2];
			if (acceleration[2]!=0  &&  acceleration[2]!=1) acceleration[2]=-1;
			SystemParametersInfo(SPI_GETMOUSE, 0, &ORIGINALacceleration, 0);
			
			// This requires array with 3 numbers
			// Last number is option on/off
			// Set first two numbers only if wanted setting is different than current
			if (acceleration[2] >= 0  &&  ORIGINALacceleration[2] != acceleration[2])
			{
				if (ORIGINALacceleration[2] == 0)
					//cout << "acceleration is disabled, user wants to enable it\n",
					acceleration[0] = 6,
					acceleration[1] = 10;
       
				if (ORIGINALacceleration[2] == 1)
					//cout << "acceleration is enabled, user wants to disable it\n",
					acceleration[0] = 0,
					acceleration[1] = 0;
			} 
			else 
				acceleration[2] = -1;
		};

		// OFP cursor sensitivity
		if(currARG.substr(0,16)=="-OFPsensitivity="  &&  currARG.length() > 16)
		{
			istringstream iss(currARG.substr(16));
			iss >> OFPsensitivity;
			changeOFPsensitivity = true;
		};

		// Center window on the screen
		if(currARG == "-center") 
			repositionCENTER = true;
		
		// Don't change cursor position
		if(currARG == "-nosetcursor") 
			repositionCURSOR = false;
		
		// Quit lockmouse on game exit
		if(currARG == "-quit") 
			quit = true;
		
		// Minimize window right now
		if(currARG == "-minimize")
        {
			HWND lockwindow = FindWindow(NULL, "Lockmouse2");
			if (lockwindow) 
				ShowWindow(lockwindow, SW_MINIMIZE);
        };
	};


	// Display enabled parameters
	cout << "Window: " << WANTED_WINDOW << endl;

	if (repositionCENTER)
		cout << "Center window\n";
	else
	{
		if (repositionX) 
			cout << "X: " << Xcoord << endl;
		if (repositionY) 
			cout << "Y: " << Ycoord << endl;
	};

	if (!repositionCURSOR) 
		cout << "Keep cursor\n";
		
	if (ORIGINALsensitivity > 0) 
		cout << "Windows sensitivity: " << sensitivity << endl;
		
	if (acceleration[2] >= 0) 
		cout << "Windows acceleration: " << acceleration[2] << endl;
		
	if (changeOFPsensitivity) 
		cout << "OFP Sensitivity: " << OFPsensitivity << endl;
		
	if (quit) 
		cout << "Quit with application\n";

	cout << "\nHold CTRL+A to release. CTRL+C in the console to quit.\n\n";



	
	
	
	



	
	
	
	
	
	
	// Define variables
	HWND currenthwnd, hwnd=NULL;
	char windowname[1024];
	string CURRENT_WINDOW;
    
	POINT pt, ptOLD;
	RECT nc, game, gameCLIENT, desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);

	int borderL=0, borderR=0, borderB=0, borderT=0;
	bool mouseClick=true, relaunchedGAME=true;
	
	// Control variables for parameters
	bool checkCursor = true;
	int changedAcceleration=-1, changedSensitivity=-1;
	DWORD changedSensitivityPID=0, currentPID=0, lastWorkingPID=0;
	
	if (sensitivity > 0) 
		changedSensitivity = 0;

	if (acceleration[2] >= 0) 
		changedAcceleration = 0;
    
	





	// Infinite loop until CTRL + C is pressed
    while(true)
    {
		Sleep(20);

		// Get active window
		currenthwnd = GetForegroundWindow();
		if (currenthwnd != NULL  &&  currenthwnd != hwnd)			// for a new window
                hwnd = currenthwnd,
				GetWindowText(hwnd, windowname, 1024),
				GetWindowThreadProcessId(hwnd, &currentPID),
				CURRENT_WINDOW = string(windowname),
				transform(CURRENT_WINDOW.begin(), CURRENT_WINDOW.end(), CURRENT_WINDOW.begin(), ::tolower);






		// If active window isn't what we wanted OR if it's explorer ---------------------------------------
		if (CURRENT_WINDOW != WANTED_WINDOW  ||  !IsNotExplorer(currentPID))
		{
			// Unlock cursor and reset click var
			ClipCursor(NULL);
			mouseClick = true;
			
			// Bring back cursor to the original position
			if (repositionCURSOR && !checkCursor) 
				checkCursor = true,
				SetCursorPos(ptOLD.x, ptOLD.y);
			
			// Bring back original mouse sensitivity
			if (changedSensitivity == 1)
				if (SystemParametersInfo(SPI_SETMOUSESPEED, 0, (void*)ORIGINALsensitivity, 0) == 1)
					changedSensitivity = 0;
					/*cout<<"brought back sens\n";*/
			
			// Bring back original mouse acceleration
			if (changedAcceleration == 1)
				if (SystemParametersInfo(SPI_SETMOUSE, 0, (void*)ORIGINALacceleration, 0) == 1) 
					changedAcceleration = 0; 
					/*cout<<"brought back acc\n";*/
            
			// Check if application still exists
			if (lastWorkingPID != 0)
				if (!IsProcessRunning(lastWorkingPID))
				{
					relaunchedGAME = true;
					
					// if user passed -quit
					if (quit) 
					{
						// If game is flashpoint
						if (WANTED_WINDOW=="operation flashpoint"  ||  WANTED_WINDOW=="cold war assault")
						{
							// if gameRestart is not running then quit
							if (findProcess("gameRestart.exe")==0) 
								return 1; 
							else 
								lastWorkingPID = 0, 
								Sleep(1000);
						}
						// If it's not flashpoint then just quit
						else
							/*cout<<"app exit",*/
							return 1;
					};
				};
            
			continue;
		};
		// -------------------------------------------------------------------------------------------------








		// Save process ID
		lastWorkingPID = currentPID;
		
		
		// Change windows mouse sensitivity -------------------------------------------
		if (changedSensitivity == 0)
			if (SystemParametersInfo(SPI_SETMOUSESPEED, 0, (void*)sensitivity, 0) == 1)
				changedSensitivity = 1; 
				/*cout<<"changed sens\n";*/
		// --------------------------------------------------------------------------

		
		// Change mouse acceleration --------------------------------------------------
		if (changedAcceleration == 0)
			if (SystemParametersInfo(SPI_SETMOUSE, 0, (void*)acceleration, 0) == 1)
				changedAcceleration = 1; 
				/*cout<<"changed acc\n";*/
		// --------------------------------------------------------------------------

		
		// Change mouse speed in OFP --------------------------------------------------
		if (changeOFPsensitivity  &&  currentPID != changedSensitivityPID)
			if (slowDownMouse(currentPID, CURRENT_WINDOW, OFPsensitivity))
				changedSensitivityPID = currentPID;
		// --------------------------------------------------------------------------


		// Change window position -----------------------------------------------------
		if (relaunchedGAME)
		{
			relaunchedGAME = false;
			GetWindowRect(hwnd, &game);
			GetClientRect(hwnd, &gameCLIENT);

			// Calculate window border size
			borderL = abs((game.right - game.left) - gameCLIENT.right) / 2;
			borderR = borderL;
			borderB = borderL;
			borderT = abs((game.bottom - game.top) - gameCLIENT.bottom) - borderL;
			
			/*cout << "borderL: " <<  borderL << endl;
			cout << "borderR: " <<  borderR << endl;
			cout << "borderB: " <<  borderB << endl;
			cout << "borderT: " <<  borderT << endl;*/

			if (repositionCENTER || repositionX || repositionY)
			{
				// Reset coordinates if not set
				if (!repositionX) 
					Xcoord = -borderL;
					
				if (!repositionY) 
					Ycoord = -borderT;
				
				// Calculate coordinates to center window
				if (repositionCENTER)
				{
					// client.right is window width and client.bottom is window height
					Xcoord = ((desktop.right - gameCLIENT.right) / 2) - borderL;
					Ycoord = ((desktop.bottom - gameCLIENT.bottom) / 2) - borderT;
					
					/*cout << "Xcoord: " << Xcoord << endl;
					cout << "Ycoord: " << Ycoord << endl << endl;*/
				};
				
				SetWindowPos(hwnd, 0, Xcoord, Ycoord, 0, 0, SWP_NOSIZE);
				Sleep(10);

				/*GetWindowRect(hwnd, &game);
				GetClientRect(hwnd, &gameCLIENT);
				cout << "game.left: " << game.left << endl;
				cout << "game.right: " << game.right << endl;
				cout << "game.top: " << game.top << endl;
				cout << "game.bottom: " << game.bottom << endl;
				cout << "gameCLIENT.left: " << gameCLIENT.left << endl;
				cout << "gameCLIENT.right: " << gameCLIENT.right << endl;
				cout << "gameCLIENT.top: " << gameCLIENT.top << endl;
				cout << "gameCLIENT.bottom: " << gameCLIENT.bottom << endl;*/
				
				continue;
			};
		};// --------------------------------------------------------------------------


		// Change cursor position -----------------------------------------------------
		if (repositionCURSOR && checkCursor)
		{
			GetCursorPos(&ptOLD);
			GetWindowRect(hwnd, &game);
			
			// For some reason the window is repositioned if you activate it by clicking on the border
			// this condition prevents it
            if 
			(
				// If inside game window (not on borders)
				ptOLD.x > game.left+borderL && 
				ptOLD.x < game.right-borderR &&
				ptOLD.y > game.top+borderT &&
				ptOLD.y < game.bottom-borderB
				||
				// OR outside game window
				ptOLD.x < game.left ||
				ptOLD.x > game.right ||
				ptOLD.y < game.top ||
				ptOLD.y > game.bottom
            )
				// then set mouse cursor in the lower right corner			
				SetCursorPos(game.right-borderR-1, game.bottom-borderB-1);

			checkCursor = false;
		};// --------------------------------------------------------------------------

		



		
		




		// If CTRL+A isn't pressed
		if (GetAsyncKeyState(VK_CONTROL) >= 0  ||  GetAsyncKeyState('A') >= 0)
		{
			GetWindowRect(hwnd, &game);	
			GetCursorPos(&pt);
			
			int threads = 0;
			bool isOFP = WANTED_WINDOW=="operation flashpoint"  ||  WANTED_WINDOW=="cold war assault";
			if (isOFP) 
				threads = countThreads(currentPID);
            
			// If cursor is in game window
            if 
			(
				pt.x > game.left+borderL && 
				pt.x < game.right-borderR &&
				pt.y > game.top+borderT &&
				pt.y < game.bottom-borderB &&
				(!isOFP  ||  isOFP && threads>=5)
				// If it's not OFP  or OFP and has at least 5 threads
            )
            {
				// Then block it
				nc.top      = pt.y;
				nc.left     = pt.x;
				nc.right    = pt.x+1;
				nc.bottom   = pt.y+1;
				ClipCursor(&nc);
				
				// Simulate a single mouse click to activate that window
				if (mouseClick)
				{
					mouseClick = false;
					SetCapture(hwnd);
					PostMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
					Sleep(5);
					PostMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
					ReleaseCapture();
				};
            };
		}
		else 
		{
			// Otherwise release cursor
			ClipCursor(NULL);
			GetCursorPos(&ptOLD);
		};
		
    };
	return 0;
}


