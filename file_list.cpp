// Generate text file listing all the files in the current directory

#include <sstream>
#include <windows.h>
#include <fstream>
#include <vector>
using namespace std;

string Int2Str(int num)
{
    ostringstream text;
    text << num;
    return text.str();
}

string FormatError(int error)
{
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

	string ret = Int2Str(error) + " " + (string)(char*)errorText + "\n";

	if (errorText != NULL)
		LocalFree(errorText);

	return ret;
}

int browse_directory(string input_path, string input_pattern, vector<string> &container) {
	string pattern = input_path + (!input_path.empty() ? "\\" : "") + input_pattern;
	
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;
	hFile = FindFirstFile(pattern.c_str(), &FileInformation);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		int errorCode = GetLastError();
		container.push_back("Failed to list files in " + input_path + " - " + FormatError(errorCode));
		return 1;
	}
	
	int number_of_files = 0;
	
	do {
		string current_file = (string)FileInformation.cFileName;
		
		if (current_file != "."  &&  current_file != "..") {
			number_of_files++;
			string path_to_current_file = input_path + (!input_path.empty() ? "\\" : "") + current_file;

			if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				browse_directory(path_to_current_file, input_pattern, container);
			else
				container.push_back(path_to_current_file);
		}
	}
	while(FindNextFile(hFile, &FileInformation) == TRUE);
	
	// If empty directory then include its name
	if (number_of_files==0 && !input_path.empty())
		container.push_back(input_path+"\\");
	
	FindClose(hFile);
	return 0;
}

int main(int argc, char *argv[])
{
	string input_path = argc>1 ? (string)argv[1] : "";
	
	for (int i=0; i<input_path.length(); i++)
		if (input_path[i]=='/')
			input_path[i]='\\';
	
	string input_pattern = "*";
	
	if (input_path.find("*")!=string::npos || input_path.find("?")!=string::npos) {
			size_t last_item = input_path.find_last_of("\\");
		if (last_item != string::npos) {
			input_pattern = input_path.substr(last_item);
			input_path    = input_path.substr(0, last_item);
		} else {
			input_pattern = input_path;
			input_path    = "";
		}
	}
	
	if (!input_path.empty() && (input_path.substr(input_path.length()-1) == "\\"))
		input_path = input_path.substr(0, input_path.length()-1);
	
	vector<string> file_list;
	browse_directory(input_path, input_pattern, file_list);
	
	ofstream logfile;
	logfile.open("file_list.txt", ios::out | ios::trunc);
	
	for (int i=0; i<file_list.size(); i++) {
		logfile << file_list[i];
		
		if (i < file_list.size() - 1)
			logfile << endl;
	}

	logfile.close();
	return 0;
}
