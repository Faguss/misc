// OFPTextureFixer v1.1 by Faguss (ofp-faguss.com)
// Scan for paa/pac file and read their width and height
// If incorrect then resize with ImageMagick/IrfanView and convert with TexView
// Compiled with Microsoft Visual Studio 2005

#include "stdafx.h"
#include <cstdio>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <vector>

using namespace std;

// List of valid paa types for OFP
// https://community.bistudio.com/wiki/PAA_File_Format
unsigned short valid_signature[] = {
	0xFF01,
	0x4444,
	0x1555,
	0x8080
};

wstring format_name[] = {
	L"dxt1",
	L"rgba4444",
	L"rgba5551",
	L"ia88"
};

unsigned short invalid_signature[] = {
	0xFF02,
	0xFF03,
	0xFF04,
	0xFF05,
	0x8888
};

string invalid_signature_name[] = {
	"DXT2 : Oxygen 2 Only",
	"DXT3",
	"DXT4 : Oxygen 2 Only",
	"DXT5 : Arma1 & 2 Only",
	"RGBA 8:8:8:8 : Oxygen 2 Only"
};

unsigned short valid_signature_num   = sizeof(valid_signature) / sizeof(valid_signature[0]);
unsigned short invalid_signature_num = sizeof(invalid_signature) / sizeof(invalid_signature[0]);

enum PROGRAM_PATHS {
	PATH_PAL2PACE,
	PATH_PAL2PACE_CFG,
	PATH_TEXVIEW,
	PATH_MAGICK,
	PATH_7ZIP,
	PATH_EXTRACTPBO,
	PATH_IRFANVIEW,
	PATH_199_STEAM,
	PATH_199,
	PATH_196,
	PATH_NUM
};

enum REGISTRY_LIST_ARRAY {
	REG_PATH,
	REG_KEY
};

wstring registry_list[][128] = {
	{L"SOFTWARE\\BIStudio\\TextureConvert",L"MAIN"},
	{L"SOFTWARE\\BIStudio\\TextureConvert",L"configPath"},
	{L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\TexView 2",L"UninstallString"},
	{L"SOFTWARE\\ImageMagick\\Current",L"BinPath"},
	{L"SOFTWARE\\7-Zip",L"Path"},
	{L"SOFTWARE\\Mikero\\ExtractPbo",L"exe"},
	{L"SOFTWARE\\IrfanView\\shell\\open\\command",L""},
	{L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 65790",L"InstallLocation"},
	{L"SOFTWARE\\Bohemia Interactive Studio\\ColdWarAssault",L"MAIN"},
	{L"SOFTWARE\\Codemasters\\Operation Flashpoint",L"MAIN"}
};





	// https://stackoverflow.com/questions/6691555/converting-narrow-string-to-wide-string
wstring string2wide(const string& input)
{
    if (input.empty())
		return wstring();

	size_t output_length = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), 0, 0);
	wstring output(output_length, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), &output[0], (int)input.length());
	
	return output;
}

	// https://mariusbancila.ro/blog/2008/10/20/writing-utf-8-files-in-c/
string wide2string(const wchar_t* input, int input_length)
{
	int output_length = WideCharToMultiByte(CP_UTF8, 0, input, input_length, NULL, 0, NULL, NULL);
      
	if (output_length == 0) 
		return "";
 
	string output(output_length, ' ');
	WideCharToMultiByte(CP_UTF8, 0, input, input_length, const_cast< char* >(output.c_str()), output_length, NULL, NULL); 
 
	return output;
}

string wide2string(const wstring& input)
{
   return wide2string(input.c_str(), (int)input.size());
}

bool IsPowerOfTwo(unsigned short number) {
	return (number & (number - 1)) == 0;
}

// https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c#315463
bool Equals(const wstring& a, const wstring& b) 
{
	size_t sz = a.size();

	if (b.size() != sz)
		return false;

	for (size_t i=0; i<sz; ++i)
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

	// Delete file or directory with its contents  http://stackoverflow.com/a/10836193
DWORD DeleteDirectory(const wstring &refcstrRootDirectory, bool bDeleteSubdirectories=true)
{
	bool bSubdirectory = false;          // Flag, indicating whether subdirectories have been found
	HANDLE hFile;                        // Handle to directory
	wstring	strFilePath;                 // Filepath
	wstring strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;     // File information

	strPattern = refcstrRootDirectory + L"\\*.*";
	hFile      = FindFirstFile(strPattern.c_str(), &FileInformation);
	
	if (hFile != INVALID_HANDLE_VALUE) {
		do {
			if (FileInformation.cFileName[0] != '.') {
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + L"\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (bDeleteSubdirectories) {
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					} else
						bSubdirectory = true;
				} else {
					// Set file attributes
					if (SetFileAttributes(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
						return GetLastError();

					// Delete file
					if (DeleteFile(strFilePath.c_str()) == FALSE)
						return GetLastError();
				}
			}
		}
		while (FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		FindClose(hFile);

		DWORD dwError = GetLastError();
		
		if (dwError != ERROR_NO_MORE_FILES)
      		return dwError;
		else {
			if (!bSubdirectory) {
				// Set directory attributes
				if (SetFileAttributes(refcstrRootDirectory.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
					return GetLastError();

				// Delete directory
				if (RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return GetLastError();
			}
		}
	}

	return 0;
}

int BrowseDirectory(wstring input_path, wstring input_pattern, vector<string> &file_log, vector<wstring> &addon_list, vector<wstring> &program_path, int &texture_files_num, wchar_t *current_directory) {	
	wstring search_pattern = input_path + (!input_path.empty() ? L"\\" : L"") + input_pattern;
	
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;
	hFile = FindFirstFile(search_pattern.c_str(), &FileInformation);

	if (hFile == INVALID_HANDLE_VALUE) {
		int errorCode = GetLastError();
		file_log.push_back("Failed to list files in " + wide2string(input_path) + " - " + FormatError(errorCode));
		return 1;
	}
	
	do {
		wstring current_file = (wstring)FileInformation.cFileName;

		if (current_file != L"."  &&  current_file != L"..") {
			bool is_dir     = (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
			size_t last_dot = current_file.find_last_of('.');
			
			if (last_dot == wstring::npos  &&  !is_dir)
				continue;
			
			wstring file_extension    = current_file.substr(last_dot+1);
			wstring current_file_path = input_path + (!input_path.empty() ? L"\\" : L"") + current_file;

			if (is_dir) {
				if (current_file != L"__textures_copy")
					BrowseDirectory(current_file_path, input_pattern, file_log, addon_list, program_path, texture_files_num, current_directory);
			} else 
			if (Equals(file_extension,L"paa") || Equals(file_extension,L"pac")) {

				texture_files_num++;

				string message        = "";
				int signature_index   = -1;
				unsigned short width  = 0;
				unsigned short height = 0;
				unsigned short ratio  = 0;
				FILE *file;
				errno_t fopen_error = _wfopen_s(&file, current_file_path.c_str(), L"rb");
				
				if (fopen_error == 0) {
					char tagg[5]                    = "";
					unsigned short signature        = 0;
					unsigned short palette_triplets = 0;
					
					fseek(file, 0, SEEK_END);
					int file_size = ftell(file);
					fseek(file, 0, SEEK_SET);
					
					if (file_size < 6) {
						message = "file too small";
						goto end_parsing;
					}
					
					// Read type of paa/pac file
					fread(&signature, 2, 1, file);
					
					for (int i=0; i<valid_signature_num && signature_index<0; i++)
						if (signature == valid_signature[i])
							signature_index = i;

					// OFP index palettes do not have a signature but start with taggs
					if (signature_index < 0) {
						fseek(file, 0, SEEK_SET);
						fread(&tagg, 4, 1, file);

						if (strcmp(tagg,"GGAT") == 0)
							signature_index = 0;
						else {
							message   += "invalid signature ";
							bool found = false;
							
							for (int j=0; j<invalid_signature_num; j++)
								if (signature == invalid_signature[j]) {
									message += invalid_signature_name[j];
									found = true;
									break;
								}
									
							if (!found) {
								char tagg_hex[8] = "";
								sprintf_s(tagg_hex, 8, "0x%x", signature);
								message += (string)tagg_hex;
							}

							goto end_parsing;
						}
					} else
						fread(&tagg, 4, 1, file);

					// Iterate on taggs
					while (strcmp(tagg,"GGAT") == 0) {
						int current_pos = ftell(file);
						
						if (current_pos+8 > file_size) {
							message = "corrupt file";
							goto end_parsing;
						}

						fseek(file, 4, SEEK_CUR);
						int data_len = 0;
						fread(&data_len, 4, 1, file);

						if (ftell(file)+data_len+4 > file_size) {
							message = "corrupt file";
							goto end_parsing;
						}

						fseek(file, data_len, SEEK_CUR);
						fread(&tagg, 4, 1, file);
					}

					// Read palette information
					fseek(file, -4, SEEK_CUR);
					fread(&palette_triplets, 2, 1, file);
					
					if (ftell(file)+palette_triplets*3 > file_size) {
						message = "corrupt file";
						goto end_parsing;
					}
					
					fseek(file, palette_triplets*3, SEEK_CUR);
					
					if (ftell(file)+8 > file_size) {
						message = "corrupt file";
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
						ratio = width / height;
					else
						ratio = height / width;

					if (!IsPowerOfTwo(width) || !IsPowerOfTwo(height) || ratio > 8) {
						message = Int2Str(width) + "x" + Int2Str(height);
					}

					end_parsing:
					fclose(file);
				} else {
					char error_message[256] = "";
					strerror_s(error_message, 256, fopen_error);
					message = (string)error_message;
				}


				// Convert texture if ratio is incorrect; unfortunately Pal2PacE.exe will not open the file if the size is not ^2
				if (ratio > 8  &&  IsPowerOfTwo(width) && IsPowerOfTwo(height)) {
					unsigned short *sizelist[2] = {&width, &height};
					
					// Find new dimensions
					// https://stackoverflow.com/questions/4398711/round-to-the-nearest-power-of-two/4398799
					for (int i=0; i<2; i++) {
						unsigned short *current = sizelist[i];
						
						if (!IsPowerOfTwo(*current)) {
							int next = *current; 

							next--;
							next |= next >> 1;
							next |= next >> 2;
							next |= next >> 4;
							next |= next >> 8;
							next |= next >> 16;
							next++; // next power of 2

							int previous = next >> 1; // previous power of 2

							*current = (next - *current) > (*current - previous) ? previous : next;
							
							if (*current < 2)
								*current = 2;
						}
					}
					
					// Correct aspect ratio
					if (width > height) {
						if (width / height > 8)
							width = height * 8;
					} else
						if (height > width)
							if (height / width > 8)
								height = width * 8;


					if (!program_path[PATH_PAL2PACE].empty()) {
						wstring new_file_name = input_path + (!input_path.empty() ? L"\\" : L"") + current_file.substr(0, last_dot+1) + format_name[signature_index] + L".";
						
						// Use pal2pace to convert to tga
						wstring command_line  = L"\"\"" + program_path[PATH_PAL2PACE] + L"\\Pal2PacE.exe\" \"" + current_file_path + L"\" \"" + new_file_name + L"tga\"\"";
						_wsystem(command_line.c_str());
						
						// Use imagemagick/irfanview to resize
						if (!program_path[PATH_MAGICK].empty() || !program_path[PATH_IRFANVIEW].empty()) {
							if (!program_path[PATH_MAGICK].empty()) {
								command_line = L"\"\"" + program_path[PATH_MAGICK] + L"\\magick.exe\" \"" + new_file_name + L"tga\" -resize " + string2wide(Int2Str(width)) + L"x" + string2wide(Int2Str(height)) + L"! \"" + new_file_name + L"tga\"\"";
								_wsystem(command_line.c_str());
							} else						
								if (!program_path[PATH_IRFANVIEW].empty()) {
									wstring full_path = (wstring)current_directory + L"\\" + new_file_name;
									command_line = L"\"\"" + program_path[PATH_IRFANVIEW] + L"\" \"" + full_path + L"tga\" /resize=(" + string2wide(Int2Str(width)) + L"," + string2wide(Int2Str(height)) + L") /resample \"/convert=" + full_path + L"tga\"\"";
									_wsystem(command_line.c_str());
								}
							
							// Use pal2pace to convert to paa
							command_line  = L"\"\"" + program_path[PATH_PAL2PACE] + L"\\Pal2PacE.exe\" \"" + new_file_name + L"tga\" \"" + new_file_name + L"paa\"\"";
							_wsystem(command_line.c_str());
							
							// Replace old file
							if (MoveFileEx((new_file_name+L"paa").c_str(), current_file_path.c_str(), MOVEFILE_REPLACE_EXISTING))
								message += " - automatically fixed";
								
							DeleteFile((new_file_name+L"tga").c_str());
							
							
							// Find addon name
							wstring current_addon_name = L"";
							
							if (!input_path.empty()) {
								current_addon_name = input_path;
								size_t pos         = input_path.find(L"addons\\");
									
								if (pos!=string::npos && (pos==0 || pos>0 && input_path[pos-1]=='\\')) {
									size_t next_slash = input_path.find(L"\\", pos+7);
									
									if (next_slash != string::npos) {
										size_t start       = pos+7;
										current_addon_name = input_path.substr(0, next_slash);
									}
								}
							}
						
							
							// Add this addon to the list
							if (!current_addon_name.empty()) {
								bool found = false;
								
								for (size_t i=0; i<addon_list.size() && !found; i++)
									if (Equals(current_addon_name,addon_list[i]))
										found = true;
										
								if (!found)
									addon_list.push_back(current_addon_name);


								// Make a copy of the texture in a separate directory
								wstring copy_path = L"__textures_copy\\" + current_file_path;
								size_t offset     = 0;
								size_t dir_pos    = copy_path.find(L"\\", offset);
								
								while (dir_pos != wstring::npos) {
									wstring dir = copy_path.substr(0, dir_pos);

									CreateDirectory(dir.c_str(), NULL);

									offset  = dir_pos + 1;
									dir_pos = copy_path.find(L"\\", offset);
								}

								CopyFile(current_file_path.c_str(), copy_path.c_str(), false);
							}
						}
					}
				}
				
				if (!message.empty()) {
					message = wide2string(current_file_path) + " - " + message;
					file_log.push_back(message);
				}
			}
		}
	}
	while(FindNextFile(hFile, &FileInformation) == TRUE);
	
	FindClose(hFile);
	return 0;
}

int wmain(int argc, wchar_t *argv[])
{
	// Check which programs are installed
	REGSAM registry_flags[] = {
		0,
		KEY_WOW64_32KEY,
		KEY_WOW64_64KEY
	};
	int registry_flags_num = sizeof(registry_flags) / sizeof(registry_flags[0]);
	
	vector<wstring> program_path;
	
	for (int i=0; i<PATH_NUM; i++) {
		program_path.push_back(L"");

		HKEY key_handle    = 0;
		wchar_t path[1024] = L"";
		DWORD path_size    = sizeof(path);
		LONG result        = 1;

		for (int j=0; j<registry_flags_num && result!=ERROR_SUCCESS; j++)
			result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, registry_list[i][REG_PATH].c_str(), 0, KEY_READ | registry_flags[j], &key_handle);

		if (result == ERROR_SUCCESS) {
			DWORD data_type = REG_SZ;
			LONG result2    = RegQueryValueEx(key_handle, registry_list[i][REG_KEY].c_str(), 0, &data_type, (BYTE*)path, &path_size);

			if (result2 == ERROR_SUCCESS) {
				program_path[i] = (wstring)path;
				
				if (program_path[i][0] == L'"' && program_path[i][program_path[i].length()-1]==L'"')
					program_path[i] = program_path[i].substr(1, program_path[i].length()-2);
			}

			RegCloseKey(key_handle);
		}
	}


	// Replace TexView2 config
	bool replaced_cfg       = false;
	wstring cfg_source      = program_path[PATH_PAL2PACE_CFG];
	wstring cfg_destination = L"";

	if (!cfg_source.empty()) {
		int tries      = 1;
		int last_error = 0;

		do {
			cfg_destination = cfg_source + L"_renamed" + (tries>1 ? string2wide(Int2Str(tries)) : L"");

			if (MoveFileEx(cfg_source.c_str(), cfg_destination.c_str(), 0)) {
				replaced_cfg = true;
				last_error   = 0;

				ofstream cfg;
				cfg.open(cfg_source.c_str(), ios::out | ios::trunc);
				
				if (cfg.is_open()) {
					cfg << "convertVersion=6;class TextureHints{class dxt1{name=\"*.dxt1.*\";format=\"DXT1\";dynRange=1;};class rgba5551{name=\"*.rgba5551.*\";format=\"ARGB1555\";dynRange=0;};class rgba4444{name=\"*.rgba4444.*\";format=\"ARGB4444\";};class ia88{name=\"*.ia88.*\";format=\"AI88\";};};";
					cfg.close();
				} else
					program_path[PATH_PAL2PACE] = L"";
			} else {
				tries++;
				last_error = GetLastError();
				
				if (last_error != ERROR_ALREADY_EXISTS)
					program_path[PATH_PAL2PACE] = L"";
			}
		} while (last_error == ERROR_ALREADY_EXISTS);
	}

	if (program_path[PATH_PAL2PACE].find(L"Arma 3 Tools") != string::npos)
		if (!program_path[PATH_TEXVIEW].empty())
			program_path[PATH_PAL2PACE] = program_path[PATH_TEXVIEW].substr(0, program_path[PATH_TEXVIEW].length()-13);
		else
			program_path[PATH_PAL2PACE] = L"";
	
	
	// Find which string contains path to the game
	int game_index = -1;
	
	for (int i=PATH_199_STEAM; i<PATH_NUM; i++)
		if (!program_path[i].empty()) {
			game_index = i;
			break;
		}
	
	
	
	// Parse arguments
	wstring input_path = L"";
	bool extract_pbo   = false;
	
	for (int i=1; i<argc; i++) {
		if (Equals((wstring)argv[i],L"-unpbo")) {
			extract_pbo = true;
		} else
			if (input_path.empty())
				input_path = (wstring)argv[i];
	}
	
	for (size_t i=0; i<input_path.length(); i++)
		if (input_path[i]==L'/')
			input_path[i]=L'\\';
	
	wstring input_pattern = L"*";
	
	if (input_path.find(L"*")!=wstring::npos || input_path.find(L"?")!=wstring::npos) {
		size_t last_item = input_path.find_last_of(L"\\");
		
		if (last_item != wstring::npos) {
			input_pattern = input_path.substr(last_item);
			input_path    = input_path.substr(0, last_item);
		} else {
			input_pattern = input_path;
			input_path    = L"";
		}
	}
	
	if (!input_path.empty() && (input_path.substr(input_path.length()-1) == L"\\"))
		input_path = input_path.substr(0, input_path.length()-1);
	
	
	
	// Unpack addons
	if (extract_pbo) {
		wstring extractor = L"";
		
		if (!program_path[PATH_EXTRACTPBO].empty())
			extractor = program_path[PATH_EXTRACTPBO];
		else
			if (game_index >= 0)
				extractor = program_path[game_index] + L"\\fwatch\\data\\extractpbo.exe";
				
		if (!extractor.empty()) {
			wstring destination = L".";
			
			if (!input_path.empty())
				destination = L"\"" + input_path + L"\"";
			
			wstring command_line = L"\"\"" + extractor + L"\" -NYP " + destination + L"\"";
			_wsystem(command_line.c_str());
		}
	}
	
	
	
	// Run file search
	wchar_t current_directory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, current_directory);
	vector<string> file_log;
	vector<wstring> addon_list;
	int texture_files_num = 0;

	DeleteDirectory(L"__textures_copy");
	BrowseDirectory(input_path, input_pattern, file_log, addon_list, program_path, texture_files_num, current_directory);
	
	
	
	// Save log describing individual files
	ofstream logfile;
	logfile.open("OFPTextureFixer_log.txt", ios::out | ios::trunc);
	
	if (logfile.is_open()) {
		logfile << texture_files_num << " texture files in total. " << file_log.size() << " are invalid" << endl << endl;
		
		for (size_t i=0; i<file_log.size(); i++) {
			logfile << file_log[i];
			
			if (i < file_log.size() - 1)
				logfile << endl;
		}
	
		logfile.close();
	}
	


	// Generate batch script for packing addons
	// and installation script for the OFP Game Schedule
	if (addon_list.size() > 0) {
		string packing_commands = "";
		wstring top_folder      = L"";
		wstring top_folder_last = L"";
		ofstream script_batch;
		ofstream script_installation;
		script_installation.open("OFPTextureFixer_GS.txt", ios::out | ios::trunc);

		if (game_index >= 0) {
			script_batch.open("OFPTextureFixer_makepbo.bat", ios::out | ios::trunc);
			
			if (script_batch.is_open())
				script_batch << "@echo off" << endl << endl;
		}
		
		
		for (size_t i=0; i<addon_list.size(); i++) {
			if (script_installation.is_open()) {
				top_folder   = addon_list[i];
				size_t slash = top_folder.find(L"\\");
	
				if (slash != string::npos)
					top_folder = top_folder.substr(0, slash);
				
				if (top_folder != top_folder_last) {
					top_folder_last = top_folder;
					
					if (i != 0)
					 	script_installation << endl << endl;
	
					script_installation << "UNPACK http://example.com/" << wide2string(top_folder) << ".7z" << endl << endl;
					
					if (!program_path[PATH_7ZIP].empty()) {
						wstring archive_name = L"__textures_copy\\" + top_folder + L".7z";
						wstring command_line = L"\"" + program_path[PATH_7ZIP] + L"7z.exe\" a -mx9 " + archive_name + L" .\\__textures_copy\\" + top_folder;
		
						DeleteDirectory(archive_name);
						_wsystem(command_line.c_str());
					}
				}
	
				wstring destination = addon_list[i];
				size_t pos          = destination.find(L"addons\\");
	
				if (pos!=string::npos && (pos==0 || pos>0 && destination[pos-1]==L'\\'))
					destination = destination.substr(pos);
				else
					destination = L"addons\\" + destination;
	
				script_installation 
				<< "UNPBO " << wide2string(destination) << endl
				<< "MOVE " << wide2string(addon_list[i]) << "\\* " << wide2string(destination) << " /match_dir" << endl
				<< "MAKEPBO";
	
				if (i < addon_list.size() - 1)
					script_installation << endl << endl;			
			}
			
			
			if (script_batch.is_open()) {
				script_batch << "\"" << wide2string(program_path[game_index]) << "\\fwatch\\data\\makepbo.exe\" -rnkz ogg,bin,wss,jpg " << wide2string(addon_list[i]) << endl;
	
				string parent_folder = "";
				size_t pos           = addon_list[i].find_last_of(L'\\');
					
				if (pos != string::npos)
					parent_folder = wide2string(addon_list[i].substr(0, pos+1));
	
				if (!program_path[PATH_7ZIP].empty())
					packing_commands += "\"" + wide2string(program_path[PATH_7ZIP]) + "7z.exe\" a -mx9 " + parent_folder + "OFPTextureFixer_addons.7z .\\" +  wide2string(addon_list[i]) + ".pbo\n";
				
				if (i == addon_list.size() - 1)
					script_batch << endl << packing_commands;
			}
		}
		
		if (script_installation.is_open())
			script_installation.close();
		
		if (script_batch.is_open())
			script_batch.close();
	}
	
	
	
	// Bring back original TexView2 config
	if (replaced_cfg)
		MoveFileEx(cfg_destination.c_str(), cfg_source.c_str(), MOVEFILE_REPLACE_EXISTING);

	return 0;
}

