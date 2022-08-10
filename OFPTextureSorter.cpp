// OFPTextureSorter v1.0 by Faguss (ofp-faguss.com) 10.08.2022
// Scan for paa/pac file and move them into different folders based on their type
// Logs textures to texture_list.csv

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <windows.h>
using namespace std;

// List of valid paa types for OFP
// https://community.bistudio.com/wiki/PAA_File_Format
unsigned short valid_signature[] = {
	0xFF01,
	0x4444,
	0x1555,
	0x8080
};
unsigned short valid_signature_num   = sizeof(valid_signature) / sizeof(valid_signature[0]);

string signature_to_folder[] = {
	"DXT1",
	"RGBA4444",
	"RGBA5551",
	"IA88"
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

int BrowseDirectory(string input_path_original, string input_path, string input_pattern, fstream &logfile) {
	string search_pattern = input_path + (!input_path.empty() ? "\\" : "") + input_pattern;
	
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;
	hFile = FindFirstFile(search_pattern.c_str(), &FileInformation);

	if (hFile == INVALID_HANDLE_VALUE) {
		cout << "Invalid handle value" << endl;
		int errorCode = GetLastError();
		return 1;
	}

	do {
		string current_file = (string)FileInformation.cFileName;
		
		if (current_file != "."  &&  current_file != "..") {
			bool is_dir     = FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			size_t last_dot = current_file.find_last_of('.');
			
			if (last_dot == string::npos  &&  !is_dir)
				continue;
			
			string file_extension    = current_file.substr(last_dot+1);
			string current_file_path = input_path + (!input_path.empty() ? "\\" : "") + current_file;

			if (is_dir) {
				BrowseDirectory(input_path_original, current_file_path, input_pattern, logfile);
			} else 
			if (Equals(file_extension,"paa") || Equals(file_extension,"pac")) {
				FILE *file = fopen(current_file_path.c_str(), "rb");
				if (file) {
					unsigned short signature = 0xFF;
					char tagg[5]             = "";
					string tags              = "";
					
					// Read type of paa/pac file
					fread(&signature, 2, 1, file);
					
					// Read taggs
					fread(&tagg, 4, 1, file);
					
					while (strcmp(tagg,"GGAT") == 0) {
						char name[5] = "";
						fread(&name, 4, 1, file);
						tags += (tags.empty() ? "" : ", ") + (string)name;
						
						unsigned long data_len = 0;
						fread(&data_len, 4, 1, file);

						fseek(file, data_len, SEEK_CUR);
						fread(&tagg, 4, 1, file);
					}
					
					fclose(file);
					
					int signature_index = -1;
					
					for (int i=0; i<valid_signature_num && signature_index<0; i++)
						if (signature == valid_signature[i])
							signature_index = i;
							
					string signature_name = (signature_index>=0 ? (string)signature_to_folder[signature_index] : "_unknown");
					
					logfile << current_file << "," << signature_name << ",\"" << tags << "\"" << endl;
					
					string source      = input_path + (!input_path.empty() ? "\\" : "") + current_file;
					string destination = input_path_original + (!input_path_original.empty() ? "\\" : "") + signature_name;

					CreateDirectory(destination.c_str(), NULL);
							
					destination += "\\" + current_file;
							
					if (!MoveFileEx(source.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING)) {
						cout << "failed to move " << source << " - " << FormatError(GetLastError()) << endl;
						continue;
					}
				} else {
					cout << "Failed to open " << current_file << " - " << strerror(errno) << endl;
				}
			}
		}
	} while(FindNextFile(hFile, &FileInformation) == TRUE);
	
	FindClose(hFile);
	return 0;
}


int main(int argc, char *argv[])
{	
	string input_path = "";
	
	for (int i=1; i<argc; i++) {
		if (input_path.empty())
			input_path = (string)argv[i];
	}
	
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
		
	fstream logfile;
	string logfile_path = input_path + (!input_path.empty() ? "\\" : "") + "texture_list.csv";
	logfile.open(logfile_path.c_str(), ios::out | ios::app);

	BrowseDirectory(input_path, input_path, input_pattern, logfile);
	
	logfile.close();
	return 0;
}
