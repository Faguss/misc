// sortMissions by Faguss (ofp-faguss.com) for Fwatch v1.16
// Program managing modfolder SP and MP missions
// Currently obsolete because the functionality was added to the Fwatch itself

#include <fstream>		// file operations
#include <iostream>		// displaying text
#include <vector>		// vectors
#include <windows.h>	// winapi
#include <tlhelp32.h>	// process/module traversing
#include <algorithm>	// find_if

using namespace std;

enum GAME_EXE_LIST {
	EXE,
	TITLE
};






// https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c#315463
bool Equals(const string& a, const string& b) 
{
    unsigned int sz = a.size();

    if (b.size() != sz)
        return false;

    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;

    return true;
}

	// Remove quotation marks
string UnQuote(string text)
{
	if (text.substr(text.length()-1) == "\"")
		text = text.substr(0, text.length()-1);
	
	if (text.substr(0,1) == "\"")
		text = text.substr(1);
		
	return text;	
}

void Tokenize(string text, string delimiter, vector<string> &container)
{
	bool first_item = false;
	bool inQuote    = false;
	
	// Split line into parts
	for (int pos=0, begin=-1;  pos<=text.length();  pos++) {
		bool isToken = pos == text.length();
		
		for (int i=0;  !isToken && i<delimiter.length();  i++)
			if (text.substr(pos,1) == delimiter.substr(i,1))
				isToken = true;
				
		if (text.substr(pos,1) == "\"")
			inQuote = !inQuote;
			
		// Mark beginning of the word
		if (!isToken  &&  begin<0)
			begin = pos;
						
		// Mark end of the word
		if (isToken  &&  begin>=0  &&  !inQuote) {
			string part = UnQuote(text.substr(begin, pos-begin));
			container.push_back(part);
			begin = -1;
		}
	}
}

	// Check if process with given ID number exists
bool IsProcessRunning (DWORD pid)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize       = sizeof(processInfo);
	bool result              = false;
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (processesSnapshot != INVALID_HANDLE_VALUE) {
		Process32First(processesSnapshot, &processInfo);
		
		do {
			if ((DWORD)processInfo.th32ProcessID == pid) 
				result = true;
		} while (Process32Next(processesSnapshot, &processInfo)  &&  !result);

		CloseHandle(processesSnapshot);
	};

	return result;
};

DWORD findOFPwindow(string &exe_name)
{
	vector<string> game_title;
	game_title.push_back("Arma Resistance");
	game_title.push_back("Cold War Assault");
	game_title.push_back("Operation Flashpoint");

	vector<string> game_exe[3];
	game_exe[0].push_back("armaresistance_server.exe");
	game_exe[0].push_back("ArmAResistance.exe");

	game_exe[1].push_back("coldwarassault_server.exe");
	game_exe[1].push_back("ColdWarAssault.exe");

	game_exe[2].push_back("ofpr_server.exe");
	game_exe[2].push_back("flashpointresistance.exe");
	game_exe[2].push_back("ofp.exe");
	game_exe[2].push_back("flashpointbeta.exe");
	game_exe[2].push_back("operationflashpoint.exe");
	game_exe[2].push_back("operationflashpointbeta.exe");

	HWND hwnd = NULL;
	hwnd      = GetTopWindow(hwnd);
	
	if (!hwnd) 
		return 0;
		
	char window_name_c[1024] = "";
	DWORD pid                = 0;

	while(hwnd) {
		GetWindowText(hwnd, window_name_c, 1024);
		string window_name = (string)window_name_c;

		// If window name has game name
		for (int i=0; i<game_title.size(); i++)
			if (Equals(window_name.substr(0,game_title[i].length()), game_title[i])) {
				GetWindowThreadProcessId(hwnd, &pid);
				
				// Get first module
				HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
				MODULEENTRY32 xModule;
	 
				if (hSnap == INVALID_HANDLE_VALUE) 
					return 0;

				xModule.dwSize = sizeof(MODULEENTRY32);
				if (Module32First(hSnap, &xModule) == 0) {
					CloseHandle(hSnap);
					return 0;
				}
				
				// Check if window exe name is OFP exe
				for (int j=0; j<game_exe[i].size(); j++)
					if (Equals((string)xModule.szModule, game_exe[i][j])) {
						exe_name = game_exe[i][j];
						CloseHandle(hSnap);
						return pid;
					}
				
				break;
			}
		
		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
	}

	return 0;
};
// *****************************************************************************


















int main (int argc, char *argv[])
{
	// Process arguments
	DWORD fwatch_pid = 0;
	DWORD game_pid   = 0;
	bool nolaunch    = false;
	vector<string> active_mods;
		
	for (int i=1; i<argc; i++) {
		string namevalue = (string)argv[i];
		size_t separator = namevalue.find_first_of('=');
		
		if (separator != string::npos) {
			string name  = namevalue.substr(0, separator);
			string value = namevalue.substr(separator+1);
			
			// Fwatch passes game PID number
			if (Equals(name,"-pid")) {
				game_pid = atoi(value.c_str());
				continue;
			}
				
			// if Fwatch -nolaunch then we get Fwatch pid
			if (Equals(name,"-fid")) {
				fwatch_pid = atoi(value.c_str());
				continue;
			}
				
			if (Equals(name,"-mod")) {
				vector<string> mods;
				Tokenize(value, ";", mods);
				
				for (int j=0; j<mods.size(); j++)
					active_mods.push_back(mods[j]);
					
				continue;
			}
		}
		
		if (Equals(namevalue,"-nolaunch"))
			nolaunch = true;
	}
	
	if (fwatch_pid == 0) {
		fstream fwatch_info;
		fwatch_info.open("fwatch_info.sqf", ios::in);
		
		if (fwatch_info.is_open()) {
		    string data_line;
			getline(fwatch_info, data_line);
			vector<string> data_array;
			Tokenize(data_line, "[,]\" ", data_array);
			
				for (int i=0; i<data_array.size(); i++) {
					if (i == 0)
						fwatch_pid = atoi(data_array[i].c_str());
					else {
						vector<string> param_array;
						Tokenize(data_array[i], " ", param_array);
						
						for (int j=0; j<param_array.size(); j++)
							if (Equals(param_array[j],"-nolaunch"))
								nolaunch = true;
					}
				}
			
			fwatch_info.close();
		}
	}

	if (nolaunch)
		active_mods.clear();

	if (game_pid==0 && !nolaunch)
		return 0;
	
	
	vector<string>moved_files_source;
	vector<string>moved_files_destination;
	
	while (true) {
		if (nolaunch) {
			string exe_name = "";
			game_pid = findOFPwindow(exe_name);

			if (game_pid == 0) {
				Sleep(500);
				
				if (IsProcessRunning(fwatch_pid))
					continue;
				else {

					return 0;
				}
			}

			// Open game process
			SIZE_T stBytes     = 0;
			HANDLE phandle     = OpenProcess(PROCESS_ALL_ACCESS, 0, game_pid);
			int base_offset    = 0;
			int module_base    = 0;
			int module_offset  = 0;
			string module_name = "";

			if (phandle == 0) {
				CloseHandle(phandle);
				Sleep(500);
				continue;
			}

			if (Equals(exe_name,"armaresistance_server.exe") || Equals(exe_name,"coldwarassault_server.exe") || Equals(exe_name,"ofpr_server.exe")) {
				module_name   = "ijl15.dll";
				module_offset = 0x4FF20;
			} else {
				module_name = "ifc22.dll";
				module_offset = 0x2C154;
			}

			// Game arguments are stored in IFC22.dll - find it's address
			HANDLE hSnap   = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, game_pid);
			MODULEENTRY32 xModule;
			
			if (hSnap == INVALID_HANDLE_VALUE) {
				CloseHandle(hSnap);
				CloseHandle(phandle);
				Sleep(500);
				continue;
			}

			xModule.dwSize = sizeof(MODULEENTRY32);

			do {
				if (lstrcmpi(xModule.szModule, (LPCTSTR)module_name.c_str()) == 0) {
					// Read module base address
					base_offset = (int)xModule.modBaseAddr + module_offset;
					break;
				}
			} while (Module32Next(hSnap, &xModule));
			CloseHandle(hSnap);

			if (base_offset == 0) {
				CloseHandle(phandle);
				Sleep(500);
				continue;
			}

			// Find offset holding arguments
			// [[ifc22.dll + 0x2c154] + 0x0] + 0x0
			int pointer[3] = {base_offset, 0, 0}, modif[2]={0x0, 0x0};
			int max_loops  = sizeof(pointer) / sizeof(pointer[0]) - 1;

			for (int i=0; i<max_loops; i++) {
				ReadProcessMemory(phandle, (LPVOID)pointer[i], &pointer[i+1], 4, &stBytes);
				pointer[i+1] = pointer[i+1] +  modif[i];
			}

			// Read 1024 bytes from that adress
			const int parameters_length        = 1024;
			char parameters[parameters_length] = "";
			ReadProcessMemory(phandle, (LPVOID)pointer[max_loops], &parameters, parameters_length, &stBytes);

			// Untokenize (replace null with spaces)
			for (int i=0, emptyChars=0; i<parameters_length; i++) {
				if (parameters[i] == '\0') {
					emptyChars++;
					
					if (emptyChars == 2) 
						break;
						
					parameters[i] = ' ';
				} else
					emptyChars = 0;
			}

			// Process game parameters
			vector<string> parameters_array;
			Tokenize((string)parameters, " ", parameters_array);
			active_mods.clear();

			for (int i=0; i<parameters_array.size(); i++) {
				size_t separator = parameters_array[i].find_first_of('=');
				
				if (separator != string::npos) {
					string name  = parameters_array[i].substr(0, separator);
					string value = parameters_array[i].substr(separator+1);

					if (Equals(name,"-mod")) {
						vector<string> mods;
						Tokenize(value, ";", mods);
						
						for (int i=0; i<mods.size(); i++)
							active_mods.push_back(mods[i]);
					}
				}
			}
		}

		
		// Move missions folder and individual mpmissions from modfolder to game folder
		moved_files_source.clear();
		moved_files_destination.clear();
		
		for (int i=0; i<active_mods.size(); i++) {

			string source      = active_mods[i] + "\\Missions";
			string destination = "Missions\\" + active_mods[i];

			if (MoveFileEx(source.c_str(), destination.c_str(), 0)) {
				moved_files_source.push_back(source);
				moved_files_destination.push_back(destination);
			}

			WIN32_FIND_DATA fd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			string path  = active_mods[i] + "\\MPMissions\\*.pbo";
			hFind        = FindFirstFile(path.c_str(), &fd);

			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					source      = active_mods[i] + "\\MPMissions\\" + fd.cFileName;
					destination = "MPMissions\\" + string(fd.cFileName);
					
					if (MoveFileEx(source.c_str(), destination.c_str(), 0)) {
						moved_files_source.push_back(source);
						moved_files_destination.push_back(destination);
					}
				} while (FindNextFile(hFind, &fd) != 0);
				FindClose(hFind);
			}
		}


		// Wait for the game to end and move files back
		while (IsProcessRunning(game_pid))
			Sleep(100);

		for (int i=0; i<moved_files_source.size(); i++)
			MoveFileEx(moved_files_destination[i].c_str(), moved_files_source[i].c_str(), 0);

		if (!nolaunch)
			break;
	}

	return 0;
};
