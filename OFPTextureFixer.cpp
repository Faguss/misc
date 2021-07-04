// OFPTextureFixer v1.01 by Faguss (ofp-faguss.com)
// Scan for paa/pac file and read their width and height
// If incorrect then resize with ImageMagick/IrfanView and convert with TexView

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

string format_name[] = {
	"dxt1",
	"rgba4444",
	"rgba5551",
	"ia88"
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
	PATH_MAGICK,
	PATH_7ZIP,
	PATH_EXTRACTPBO,
	PATH_199_STEAM,
	PATH_199,
	PATH_196,
	PATH_IRFANVIEW,
	PATH_NUM
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

	// Delete file or directory with its contents  http://stackoverflow.com/a/10836193
int DeleteDirectory(const string &refcstrRootDirectory, bool bDeleteSubdirectories=true)
{
	bool            bSubdirectory = false;       // Flag, indicating whether subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	string     	    strFilePath;                 // Filepath
	string          strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information

	strPattern = refcstrRootDirectory + "\\*.*";
	hFile      = FindFirstFile(strPattern.c_str(), &FileInformation);
	
	if (hFile != INVALID_HANDLE_VALUE) {
		do {
			if (FileInformation.cFileName[0] != '.') {
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

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

int BrowseDirectory(string input_path, string input_pattern, vector<string> &file_log, vector<string> &addon_list, vector<string> &program_path, int &texture_files_num, TCHAR *current_directory) {	
	string search_pattern = input_path + (!input_path.empty() ? "\\" : "") + input_pattern;
	
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;
	hFile = FindFirstFile(search_pattern.c_str(), &FileInformation);

	if (hFile == INVALID_HANDLE_VALUE) {
		int errorCode = GetLastError();
		file_log.push_back("Failed to list files in " + input_path + " - " + FormatError(errorCode));
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
				if (current_file != "__textures_copy")
					BrowseDirectory(current_file_path, input_pattern, file_log, addon_list, program_path, texture_files_num, current_directory);
			} else 
			if (Equals(file_extension,"paa") || Equals(file_extension,"pac")) {

				texture_files_num++;

				string message        = "";
				int signature_index   = -1;
				unsigned short width  = 0;
				unsigned short height = 0;
				unsigned short ratio  = 0;
				FILE *file            = fopen(current_file_path.c_str(), "rb");
				
				if (file) {
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
								sprintf(tagg_hex, "0x%x", signature);
								message += (string)tagg_hex;
							}

							goto end_parsing;
						}
					} else
						fread(&tagg, 4, 1, file);

					// Iterate on taggs
					while (strcmp(tagg,"GGAT") == 0) {
						if (ftell(file)+8 > file_size) {
							message = "corrupt file";
							goto end_parsing;
						}

						fseek(file, 4, SEEK_CUR);
						unsigned long data_len = 0;
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
				} else
					message = (string)strerror(errno);


				// Convert texture if size ratio is incorrect; unfortunately Pal2PacE.exe will not open file if size is not ^2
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
						string new_file_name = input_path + (!input_path.empty() ? "\\" : "") + current_file.substr(0, last_dot+1) + format_name[signature_index] + ".";
						
						// Use pal2pace to convert to tga
						string command_line  = "\"\"" + program_path[PATH_PAL2PACE] + "\\Pal2PacE.exe\" \"" + current_file_path + "\" \"" + new_file_name + "tga\"\"";
						system(command_line.c_str());
						
						// Use imagemagick to resize image
						if (!program_path[PATH_MAGICK].empty() || !program_path[PATH_IRFANVIEW].empty()) {
							if (!program_path[PATH_MAGICK].empty()) {
								command_line = "\"\"" + program_path[PATH_MAGICK] + "\\magick.exe\" \"" + new_file_name + "tga\" -resize " + Int2Str(width) + "x" + Int2Str(height) + "! \"" + new_file_name + "tga\"\"";
								system(command_line.c_str());
							} else						
								if (!program_path[PATH_IRFANVIEW].empty()) {
									string full_path = (string)current_directory + "\\" + new_file_name;
									command_line = "\"\"" + program_path[PATH_IRFANVIEW] + "\" \"" + full_path + "tga\" /resize=(" + Int2Str(width) + "," + Int2Str(height) + ") /resample \"/convert=" + full_path + "tga\"\"";
									system(command_line.c_str());								
								}
							
							// Use pal2pace to convert to paa
							command_line  = "\"\"" + program_path[PATH_PAL2PACE] + "\\Pal2PacE.exe\" \"" + new_file_name + "tga\" \"" + new_file_name + "paa\"\"";
							system(command_line.c_str());
							
							// Replace old file
							if (MoveFileEx((new_file_name+"paa").c_str(), current_file_path.c_str(), MOVEFILE_REPLACE_EXISTING))
								message += " - automatically fixed";
								
							DeleteFile((new_file_name+"tga").c_str());
							
							
							// Find addon name
							string current_addon_name = "";
							
							if (!input_path.empty()) {
								current_addon_name = input_path;
								size_t pos         = input_path.find("addons\\");
									
								if (pos!=string::npos && (pos==0 || pos>0 && input_path[pos-1]=='\\')) {
									size_t next_slash = input_path.find("\\", pos+7);
									
									if (next_slash != string::npos) {
										size_t start       = pos+7;
										current_addon_name = input_path.substr(0, next_slash);
									}
								}
							}
						
							
							// Add this to addon to the list
							if (!current_addon_name.empty()) {
								bool found = false;
								
								for (int i=0; i<addon_list.size() && !found; i++)
									if (Equals(current_addon_name,addon_list[i]))
										found = true;
										
								if (!found)
									addon_list.push_back(current_addon_name);


								// Make a copy of the texture in a separate directory
								string copy_path = "__textures_copy\\" + current_file_path;
								size_t offset    = 0;
								size_t dir_pos   = copy_path.find("\\", offset);
								
								while (dir_pos != string::npos) {
									string dir = copy_path.substr(0, dir_pos);

									CreateDirectory(dir.c_str(), NULL);

									offset  = dir_pos + 1;
									dir_pos = copy_path.find("\\", offset);
								}

								CopyFile(current_file_path.c_str(), copy_path.c_str(), false);
							}
						}
					}
				}
				
				if (!message.empty()) {
					message = current_file_path + " - " + message;
					file_log.push_back(message);
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
	// Check which programs are installed
	string registry_paths[] = {
		"SOFTWARE\\BIStudio\\TextureConvert",
		"SOFTWARE\\ImageMagick\\Current",
		"SOFTWARE\\7-Zip",
		"SOFTWARE\\Mikero\\ExtractPbo",
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 65790",
		"SOFTWARE\\Bohemia Interactive Studio\\ColdWarAssault",
		"SOFTWARE\\Codemasters\\Operation Flashpoint",
		"SOFTWARE\\IrfanView\\shell\\open\\command"
	};
	
	string registry_keys[] = {
		"MAIN",
		"BinPath",
		"Path",
		"exe",
		"InstallLocation",
		"MAIN",
		"MAIN",
		""
	};
	
	REGSAM registry_flags[] = {
		0,
		KEY_WOW64_64KEY,
		KEY_WOW64_64KEY,
		0,
		0,
		0,
		0,
		KEY_WOW64_64KEY
	};
	
	vector<string> program_path;
	
	for (int i=0; i<PATH_NUM; i++) {
		program_path.push_back("");

		HKEY key_handle = 0;
		char path[1024] = "";
		DWORD path_size = sizeof(path);
		LONG result     = RegOpenKeyEx(HKEY_LOCAL_MACHINE, registry_paths[i].c_str(), 0, KEY_READ | registry_flags[i], &key_handle);

		if (result == ERROR_SUCCESS) {
			DWORD data_type = REG_SZ;

			if (RegQueryValueEx(key_handle, registry_keys[i].c_str(), 0, &data_type, (BYTE*)path, &path_size) == ERROR_SUCCESS) {
				program_path[i] = (string)path;
				
				if (program_path[i][0] == '"' && program_path[i][program_path[i].length()-1]=='"')
					program_path[i] = program_path[i].substr(1, program_path[i].length()-2);
			}
		}

		RegCloseKey(key_handle);
	}

	

	// Replace TexView2 config
	bool cfg_restore       = false;
	string cfg_source      = program_path[PATH_PAL2PACE] + "\\TexConvert.cfg";
	string cfg_destination = "";
	
	if (!program_path[PATH_PAL2PACE].empty()) {
		int tries      = 1;
		int last_error = 0;
		
		do {
			cfg_destination = program_path[PATH_PAL2PACE] + "\\TexConvert.cfg_renamed" + (tries>1 ? Int2Str(tries) : "");

			if (MoveFileEx(cfg_source.c_str(), cfg_destination.c_str(), 0)) {
				cfg_restore = true;
				last_error  = 0;
				
				ofstream cfg;
				cfg.open(cfg_source.c_str(), ios::out | ios::trunc);
				
				if (cfg.is_open()) {
					cfg << "convertVersion=6;class TextureHints{class dxt1{name=\"*.dxt1.*\";format=\"DXT1\";dynRange=1;};class rgba5551{name=\"*.rgba5551.*\";format=\"ARGB1555\";dynRange=0;};class rgba4444{name=\"*.rgba4444.*\";format=\"ARGB4444\";};class ia88{name=\"*.ia88.*\";format=\"AI88\";};};";
					cfg.close();
				} else
					program_path[PATH_PAL2PACE] = "";
			} else {
				tries++;
				last_error = GetLastError();
				
				if (last_error != 183)
					program_path[PATH_PAL2PACE] = "";
			}
		} while (last_error == 183);
	}
	
	
	
	// Find which string contains path to the game
	int game_index = -1;
	
	for (int i=PATH_199_STEAM; i<PATH_NUM; i++)
		if (!program_path[i].empty()) {
			game_index = i;
			break;
		}
	
	
	
	// Parse arguments
	string input_path = "";
	bool extract_pbo  = false;
	
	for (int i=1; i<argc; i++) {
		if (Equals((string)argv[i],"-unpbo")) {
			extract_pbo = true;
		} else
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
	
	
	
	// Unpack addons
	if (extract_pbo) {
		string extractor = "";
		
		if (!program_path[PATH_EXTRACTPBO].empty())
			extractor = program_path[PATH_EXTRACTPBO];
		else
			if (game_index >= 0)
				extractor = program_path[game_index] + "\\fwatch\\data\\extractpbo.exe";
				
		if (!extractor.empty()) {
			string destination = ".";
			
			if (!input_path.empty())
				destination = "\"" + input_path + "\"";
			
			string command_line = "\"\"" + extractor + "\" -NYP " + destination + "\"";
			system(command_line.c_str());
		}
	}
	
	
	
	// Run file search
	TCHAR current_directory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, current_directory);
	vector<string> file_log;
	vector<string> addon_list;
	int texture_files_num = 0;

	DeleteDirectory("__textures_copy");
	BrowseDirectory(input_path, input_pattern, file_log, addon_list, program_path, texture_files_num, current_directory);
	
	
	
	// Save log describing individual files
	ofstream logfile;
	logfile.open("OFPTextureFixer_log.txt", ios::out | ios::trunc);
	
	if (logfile.is_open()) {
		logfile << texture_files_num << " texture files in total. " << file_log.size() << " are invalid" << endl << endl;
		
		for (int i=0; i<file_log.size(); i++) {
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
		string top_folder       = "";
		string top_folder_last  = "";
		ofstream script_batch;
		ofstream script_installation;
		script_installation.open("OFPTextureFixer_GS.txt", ios::out | ios::trunc);

		if (game_index >= 0) {
			script_batch.open("OFPTextureFixer_makepbo.bat", ios::out | ios::trunc);
			
			if (script_batch.is_open())
				script_batch << "@echo off" << endl << endl;
		}
		
		
		for (int i=0; i<addon_list.size(); i++) {
			if (script_installation.is_open()) {
				top_folder   = addon_list[i];
				size_t slash = top_folder.find("\\");
	
				if (slash != string::npos)
					top_folder = top_folder.substr(0, slash);
				
				if (top_folder != top_folder_last) {
					top_folder_last = top_folder;
					
					if (i != 0)
					 	script_installation << endl << endl;
	
					script_installation << "UNPACK http://example.com/" << top_folder << ".7z" << endl << endl;
					
					if (!program_path[PATH_7ZIP].empty()) {
						string archive_name = "__textures_copy\\" + top_folder + ".7z";
						string command_line = "\"" + program_path[PATH_7ZIP] + "7z.exe\" a -mx9 " + archive_name + " .\\__textures_copy\\" + top_folder;
		
						DeleteDirectory(archive_name);
						system(command_line.c_str());
					}
				}
	
				string destination = addon_list[i];
				size_t pos         = destination.find("addons\\");
	
				if (pos!=string::npos && (pos==0 || pos>0 && destination[pos-1]=='\\'))
					destination = destination.substr(pos);
				else
					destination = "addons\\" + destination;
	
				script_installation 
				<< "UNPBO " << destination << endl
				<< "MOVE " << addon_list[i] << "\\* " << destination << " /match_dir" << endl
				<< "MAKEPBO";
	
				if (i < addon_list.size() - 1)
					script_installation << endl << endl;			
			}
			
			
			if (script_batch.is_open()) {
				script_batch << "\"" << program_path[game_index] << "\\fwatch\\data\\makepbo.exe\" -rnkz ogg,bin,wss,jpg " << addon_list[i] << endl;
	
				string parent_folder = "";
				size_t pos           = addon_list[i].find_last_of('\\');
					
				if (pos != string::npos)
					parent_folder = addon_list[i].substr(0, pos+1);
	
				if (!program_path[PATH_7ZIP].empty())
					packing_commands += "\"" + program_path[PATH_7ZIP] + "7z.exe\" a -mx9 " + parent_folder + "OFPTextureFixer_addons.7z .\\" +  addon_list[i] + ".pbo\n";
				
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
	if (cfg_restore)
		MoveFileEx(cfg_destination.c_str(), cfg_source.c_str(), MOVEFILE_REPLACE_EXISTING);
	
	return 0;
}
