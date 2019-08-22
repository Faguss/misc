#include <string>
#include <windows.h>

using namespace std;

int main(int argc, char *argv[]) 
{
	if (argc <= 1) {
		printf("copy2usb.exe\nCopies selected file(s) to all removable drives\n\n\tUsage:\n\tcopy2usb <filename1> <filename2> ...\n\n");
		system("pause");
		return 0;
	};
	
	char drives_string[MAX_PATH] = "";
	char *current_drive          = drives_string;
    GetLogicalDriveStrings(MAX_PATH, drives_string);
	
	while(*current_drive) {
		int drive_type = GetDriveTypeA(current_drive);
		
		if (drive_type == DRIVE_REMOVABLE) {
			for (int i=1; i<argc; i++) {
				string source      = argv[i];
				string destination = current_drive;
				size_t lastSlash   = source.find_last_of("\\");
				
				if (lastSlash != string::npos)
					destination += source.substr(lastSlash, source.length());
				else {
					destination += source;
					TCHAR current_dir[MAX_PATH];
					GetCurrentDirectory(MAX_PATH, current_dir);
					source = (string)current_dir + "\\" + source;
				}
				
				printf("%s - ", source.c_str());
				
				if (CopyFile(source.c_str(), destination.c_str(), true))
					printf("COPIED\n");
				else {
					LPTSTR errorText = NULL;
					
					FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(), 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&errorText,
					0,
					NULL);
					
					printf("%s\n", (char*)errorText);
					
					if (errorText != NULL)
						LocalFree(errorText);
				}
			}
		}
		
		current_drive += strlen(current_drive) + 1;
    }
    
	return 0;
}

