// Program that I've used to rename a selection name in all rtm files
// Usage: binaryfile_replacestring.exe <string_to_find> <string_to_replace_with> <file_extension>
// Example: binaryfile_replacestring Head hlava rtm

#include <iostream>
#include <windows.h>

using namespace std;

struct GLOBAL_VARIABLES {
	char *text_buffer;
	int text_buffer_size;
	bool text_buffer_allocated;
} global = {
	NULL,
	0,
	false
};

string FormatError(int error) {
	if (error == 0) 
		return "\n";

	LPTSTR errorText = NULL;

	FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	NULL,
	error, 
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	(LPTSTR)&errorText,
	0,
	NULL);

	string ret = "   - " + (string)(char*)errorText + "\n";

	if (errorText != NULL)
		LocalFree(errorText);

	return ret;
}

int edit_file(string file_name, string &tofind, string &replacewith) {
	cout << file_name << " - ";
	
	FILE *file = fopen(file_name.c_str(), "rb");
	if (!file) {
		cout << "failed to open - " << errno << " " << strerror(errno) << endl;
		return 1;
	}
	
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	if (global.text_buffer_size < file_size+1) {
		if (global.text_buffer_allocated)
			delete[] global.text_buffer;
			
		global.text_buffer = new char[file_size+1];
		if (!global.text_buffer) {
			cout << "failed to allocate " << (file_size+1) << " bytes" << endl;
			return 2;
		}
		
		global.text_buffer_size      = file_size + 1;
		global.text_buffer_allocated = true;
	}
	

	int bytes_read                = fread(global.text_buffer, 1, file_size, file);
	global.text_buffer[file_size] = '\0';
	fclose(file);

	if (bytes_read != file_size) {
		cout << "reading error " << bytes_read << " != " << file_size;
		
		if (ferror(file))
			cout << " - " << errno << " " << strerror(errno);
			
		cout << endl;
		return 3;
	}
	
	int occurences = 0;
		
	for (int i=0; i<=file_size; i++) {
		char *word = global.text_buffer + i;
		
		if (strcmpi(word, tofind.c_str()) == 0) {
			memcpy(word, replacewith.c_str(), replacewith.length()+1);
			occurences++;
		}
	}
	
	cout << "replaced " << occurences << " times ";
	
    char temp[L_tmpnam];
    if (tmpnam(temp) == NULL) {
        cout << "- failed to get temp file name" << endl;
        return 4;
    }
    
    bool failure = false;
    
	if (file = fopen(temp, "wb")) {
		int bytes_written = fwrite(global.text_buffer, sizeof(*global.text_buffer), file_size, file);
		
		if (bytes_written != file_size) {
			failure = true;
			cout << "- writing error " << bytes_written << " != " << file_size;
			
			if (ferror(file))
				cout << " - " << errno << " " << strerror(errno);
		}
		
		fclose(file);
	} else {
		failure = true;
		cout << "- failed to create - " << errno << " " << strerror(errno);
	}
	
	if (failure) {
		cout << endl;
		return 5;
	}
	
	int result = DeleteFile(file_name.c_str());
	if (!result) {
		int errorCode = GetLastError();
		cout << "- failed to delete " << file_name << " - " << errorCode << " " << FormatError(errorCode) << endl;
		return 6;
	}
	
	result = MoveFileEx(temp, file_name.c_str(), 0);
    if (!result) {
		int errorCode = GetLastError();
		cout << "- failed to rename " << file_name << " - " << errorCode << " " << FormatError(errorCode) << endl;
		return 7;
    }
	
	cout << endl;
	return 0;
}


int browse_directory(string path, string &tofind, string &replacewith, string &wanted_extension) {
	string pattern = path + (!path.empty() ? "\\" : "") + "*";
	
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;
	hFile = FindFirstFile(pattern.c_str(), &FileInformation);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		int errorCode = GetLastError();
		cout << "Failed to list files in " << path << " - " << FormatError(errorCode) << endl;
		return 1;
	}
	
	do {
		string current_file = (string)FileInformation.cFileName;
		
		if (current_file != "."  &&  current_file != "..") {
			string file_name      = path + (!path.empty() ? "\\" : "") + FileInformation.cFileName;
			string file_extension = file_name.substr(file_name.find_last_of('.')+1);
			
			if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				browse_directory(file_name, tofind, replacewith, wanted_extension);
			else 
				if (file_extension == wanted_extension)
					edit_file(file_name, tofind, replacewith);
		}
	} 
	while(FindNextFile(hFile, &FileInformation) == TRUE);
	
	FindClose(hFile);
	return 0;
}


int main(int argc, char *argv[])
{
	string tofind           = "Head";
	string replacewith      = "hlava";
	string wanted_extension = "rtm";

	for (int i=1; i<argc; i++) {
		switch(i) {
			case 1 : tofind          =(string)argv[i]; break;
			case 2 : replacewith     =(string)argv[i]; break;
			case 3 : wanted_extension=(string)argv[i]; break;
		}
	}

	browse_directory("", tofind, replacewith, wanted_extension);
	
	if (global.text_buffer_allocated)
		delete[] global.text_buffer;

	return 0;
}

