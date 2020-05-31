// Generate text file listing all the paa/pac files with incorrect mipmap size
// by Faguss (ofp-faguss.com)

// https://community.bistudio.com/wiki/PAA_File_Format

#include <sstream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

unsigned short valid_signatures[] = {
	0xFF01,
	0xFF02,
	0xFF03,
	0xFF04,
	0xFF05,
	0x4444,
	0x1555,
	0x8888,
	0x8080
};
unsigned short valid_signatures_num = sizeof(valid_signatures) / sizeof(valid_signatures[0]);

int texture_files_num = 0;

vector<string> program_paths;

enum PROGRAM_PATHS {
	EXE_PAL2PACE,
	EXE_MAGICK,
	EXE_NUM
};







bool IsPowerOfTwo(unsigned short number) {
	return (number & (number - 1)) == 0;
}
			
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
	
	do {
		string current_file = (string)FileInformation.cFileName;
		
		if (current_file != "."  &&  current_file != "..") {
			size_t last_dot = current_file.find_last_of('.');
			
			if (last_dot == string::npos)
				continue;
			
			string file_extension       = current_file.substr(last_dot+1);
			string path_to_current_file = input_path + (!input_path.empty() ? "\\" : "") + current_file;

			if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				browse_directory(path_to_current_file, input_pattern, container);
			}
			else 
			if (Equals(file_extension,"paa") || Equals(file_extension,"pac")) {
				texture_files_num++;
				
				bool is_valid_signature         = false;
				unsigned short signature        = 0;
				char tagg[5]                    = "";
				char tagg_name[5]               = "";
				unsigned short palette_triplets = 0;
				unsigned short width            = 0;
				unsigned short height           = 0;
				int multiplier                  = 0;
				
				string message = "";
				bool is_alpha  = false;
				FILE *file     = fopen(path_to_current_file.c_str(), "rb");
				
				if (file) {
					
					fseek(file, 0, SEEK_END);
					int file_size = ftell(file);
					fseek(file, 0, SEEK_SET);
					
					if (file_size < 6) {
						message = "file too small";
						goto end_parsing;
					}
					
					// Read type of paa/pac file
					fread(&signature, 2, 1, file);
					
					for(int i=0; i<valid_signatures_num && !is_valid_signature; i++)
						if (signature == valid_signatures[i])
							is_valid_signature = true;
					
					if (!is_valid_signature) {
						fseek(file, 0, SEEK_SET);
						fread(&tagg, 4, 1, file);
						
						if (strcmp(tagg,"GGAT") != 0) {
							message = "invalid signature";
							goto end_parsing;
						}
					} else
						fread(&tagg, 4, 1, file);

					// Iterate on taggs
					while (strcmp(tagg,"GGAT") == 0) {
						if (ftell(file)+8 > file_size) {
							message = "invalid tagg";
							goto end_parsing;
						}

						fread(&tagg_name, 4, 1, file);

						// Check if texture has alpha channel
						if (strcmp(tagg_name,"GALF") == 0) {
							fseek(file, 4, SEEK_CUR);
							unsigned long transparency = 0;
							fread(&transparency, 4, 1, file);
							is_alpha = transparency != 0;
						} else {
							unsigned long data_len = 0;
							fread(&data_len, 4, 1, file);
	
							if (ftell(file)+data_len+4 > file_size) {
								message = "invalid tagg data len";
								goto end_parsing;
							}
	
							fseek(file, data_len, SEEK_CUR);
						}
						
						fread(&tagg, 4, 1, file);
					}

					// Read palette information
					fseek(file, -4, SEEK_CUR);
					fread(&palette_triplets, 2, 1, file);
					
					if (ftell(file)+palette_triplets*3 > file_size) {
						message = "invalid palette data len";
						goto end_parsing;
					}
					
					fseek(file, palette_triplets*3, SEEK_CUR);
					
					if (ftell(file)+8 > file_size) {
						message = "invalid size information";
						goto end_parsing;
					}

					fread(&width, 2, 1, file);
					fread(&height, 2, 1, file);

					// Skip special signature
					if (width==0x4D2 && height==0x223D) {
						fread(&width, 2, 1, file);
						fread(&height, 2, 1, file);
					}

					// Verify size
					if (width > height)
						multiplier = width / height;
					else
						multiplier = height / width;

					if (!IsPowerOfTwo(width) || !IsPowerOfTwo(height) || multiplier > 8)
						message = Int2Str(width) + "x" + Int2Str(height);

					end_parsing:
					fclose(file);
				} else
					message = (string)strerror(errno);

				if (!message.empty()) {
					message = path_to_current_file + " - " + message;
					container.push_back(message);
					
					// TO DO LATER
					if (multiplier <= 8)
						continue;
					
					string new_file_name = input_path + (!input_path.empty() ? "\\" : "") + current_file.substr(0, last_dot+1) + (is_alpha ? "tga" : "png");
					
					if (!program_paths[EXE_PAL2PACE].empty()) {
						string command_line  = "\"\"" + program_paths[EXE_PAL2PACE] + "\\Pal2PacE.exe\" \"" + path_to_current_file + "\" \"" + new_file_name + "\"\"";
						//string command_line  = "\"" + program_paths[EXE_PAL2PACE] + "\\Pal2PacE.exe\"";
						cout << command_line << endl;
						system(command_line.c_str());
					}
					
					if (!is_alpha && !program_paths[EXE_MAGICK].empty()) {
						unsigned short new_width = width > height ? height * 8 : width;
						unsigned short new_height = height > width ? width * 8 : height;
						
						cout << endl << new_width << "x" << new_height << endl;
						
						string command_line  = "\"\"" + program_paths[EXE_MAGICK] + "\\magick.exe\" \"" + new_file_name + "\" -resize " + Int2Str(new_width) + "x" + Int2Str(new_height) + "! \"" + new_file_name + "\"\"";
						cout << command_line << endl;
						system(command_line.c_str());
						
						if (!program_paths[EXE_PAL2PACE].empty()) {
							string command_line  = "\"\"" + program_paths[EXE_PAL2PACE] + "\\Pal2PacE.exe\" \"" + new_file_name + "\" \"" + path_to_current_file + "\"\"";
							cout << command_line << endl;
							system(command_line.c_str());
						}
					}
				}
			}
		}
	}
	while(FindNextFile(hFile, &FileInformation) == TRUE);
	
	FindClose(hFile);
	return 0;
}




int main(int argc, char *argv[])
{
	vector<string> registry_paths;
	vector<string> registry_keys;
	vector<REGSAM> registry_flags;
	registry_paths.push_back("Software\\BIStudio\\TextureConvert");
	registry_paths.push_back("SOFTWARE\\ImageMagick\\Current");
	registry_keys.push_back("MAIN");
	registry_keys.push_back("BinPath");
	registry_flags.push_back(KEY_WOW64_32KEY);
	registry_flags.push_back(KEY_WOW64_64KEY);
	
	for (int i=0; i<registry_paths.size(); i++) {
		program_paths.push_back("");
		
		HKEY key_handle = 0;
		char path[1024] = "";
		DWORD path_size = sizeof(path);
		
		LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,registry_paths[i].c_str(), 0, KEY_READ | registry_flags[i], &key_handle);
  
		if (result == ERROR_SUCCESS) {
			DWORD data_type = REG_SZ;
	
			if (RegQueryValueEx(key_handle, registry_keys[i].c_str(), 0, &data_type, (BYTE*)path, &path_size) == ERROR_SUCCESS)
				program_paths[i] = (string)path;
		}
	
		RegCloseKey(key_handle);
	}
	
	
	
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
	logfile.open("OFPTextureChecker.txt", ios::out | ios::trunc);
	logfile << texture_files_num << " texture files in total. " << file_list.size() << " are invalid" << endl << endl;
	
	for (int i=0; i<file_list.size(); i++) {
		logfile << file_list[i];
		
		if (i < file_list.size() - 1)
			logfile << endl;
	}

	logfile.close();
	return 0;
}
