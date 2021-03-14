// PBOfilelist v1.0 by Faguss (ofp-faguss.com)

#include <iostream>		// cout
#include <vector>		// data storage
#include <fstream>		// file operations
#include <windows.h>	// winapi
#include <sstream>      // for converting int to string
#include <math.h>		// for fmod
#include <algorithm>	// tolower

using namespace std;

//https://community.bistudio.com/wiki/PBO_File_Format
struct PBO_single_entry 
{
	unsigned long MimeType;
	unsigned long OriginalSize;
	unsigned long Offset;
	unsigned long TimeStamp;
	unsigned long Datasize;
};

enum MimeTypes 
{
	MIME_DUMMY      = 0x0,
	MIME_PROPERTIES = 0x56657273,
	MIME_COMPRESSED = 0x43707273,
	MIME_ENCRYPTED  = 0x456e6372
};

struct PBO_info_table
{
	vector<string> properties;
	vector<string> file_name;
	vector<int> file_mime;
	vector<unsigned int> file_origsize;
	vector<unsigned int> file_time;
	vector<unsigned int> file_datasize;
	string signature;
	int origsize;
	int datasize;
	time_t timestamp;
} global_saved_record;

enum SIZE_NAMES 
{
	BYTES,
	KILOBYTES,
	MEGABYTES
};

enum OPTIONS 
{
	OPTION_NONE           = 0x0,
	OPTION_TIMESTAMP      = 0x1,
	OPTION_FULLBYTES      = 0x2,
	OPTION_CSV            = 0x4,
	OPTION_FIX            = 0x8,
	OPTION_RECURSIVE      = 0x10,
	OPTION_SINGLELOG      = 0x20,
	OPTION_TRUNCATE       = 0x40,
	OPTION_RECORD_SAVE    = 0x80,
	OPTION_RECORD_COMPARE = 0x100,
	OPTION_HEADER_REMOVE  = 0x200
};

	//https://www.gamedev.net/forums/topic/565693-converting-filetime-to-time_t-on-windows/
time_t FileTimeToUnixTime(FILETIME const& ft) 
{
	ULARGE_INTEGER ull;
	ull.LowPart  = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return ull.QuadPart / 10000000ULL - 11644473600ULL;
}

	//https://support.microsoft.com/en-us/help/167296/how-to-convert-a-unix-time-t-to-a-win32-filetime-or-systemtime
FILETIME UnixTimeToFileTime(time_t t)
{
	FILETIME ft;
	LONGLONG ll       = Int32x32To64(t, 10000000) + 116444736000000000;
	ft.dwLowDateTime  = (DWORD)ll;
	ft.dwHighDateTime = ll >> 32;
	return ft;
}

string Int2Str(int num, bool leading_zero=false)
{
    ostringstream text;
    
    if (leading_zero && num<10)
    	text << "0";
    
    text << num;
    return text.str();
}

string FormatDate(FILETIME const& ft, int &options)
{
	if (options & OPTION_TIMESTAMP)
		return Int2Str(FileTimeToUnixTime(ft));
	else {
		SYSTEMTIME st;
		FileTimeToSystemTime(&ft, &st);	
		return 
			Int2Str(st.wYear) + "." + 
			Int2Str(st.wMonth,1) + "." + 
			Int2Str(st.wDay,1) + " " + 
			Int2Str(st.wHour,1) + ":" + 
			Int2Str(st.wMinute,1) + ":" + 
			Int2Str(st.wSecond,1);
	}
}

string FormatDate(time_t t, int &options)
{
	if (options & OPTION_TIMESTAMP)
		return Int2Str(t);
	else
		return FormatDate(UnixTimeToFileTime(t), options);
}

string FormatSize(unsigned long bytes, int &options)
{
	if (options & OPTION_FULLBYTES) {
		string output = Int2Str(bytes);
		
		if (~options & OPTION_CSV)
			output += (bytes!=1 ? " bytes" : " byte");
		
		return output;
	} else {
		double size[] = {(double)bytes, 0, 0};
		string name[] = {"B", "KB", "MB"};
	
		if (size[BYTES] >= 1048576) {
			size[MEGABYTES]  = size[BYTES] / 1048576;
			size[MEGABYTES] -= fmod(size[MEGABYTES], 1);
			size[BYTES]     -= size[MEGABYTES] * 1048576;
		}
	
		if (size[BYTES] >= 1024) {
			size[KILOBYTES]  = size[BYTES] / 1024;
			size[KILOBYTES] -= fmod(size[KILOBYTES], 1);
			size[BYTES]     -= size[KILOBYTES] * 1024;
		}
		
		int select = 2;
		
		while (select >= 0)
			if (size[select] == 0)
				select--;
			else
				break;
	
		double final = size[select];
		
		if (select > 0)
			final += size[select-1] / 1024;
			
		return Int2Str(round(final)) + " " + name[select];
	}
}

int FormatError(int error)
{
	if (error == 0) 
		return 0;

	LPTSTR errorText = NULL;

	FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	NULL,
	error, 
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	(LPTSTR)&errorText,
	0,
	NULL);

	cout <<  "   - " << (char*)errorText << endl;

	if (errorText != NULL)
		LocalFree(errorText);

	return error;
}

int CreatePBOLog(string path_to_pbo, int &options) 
{
	WIN32_FILE_ATTRIBUTE_DATA fd;
	if (!GetFileAttributesEx(path_to_pbo.c_str(), GetFileExInfoStandard, &fd))
		return FormatError(GetLastError());
				
				
	// Optionally correct timestamps in a PBO
	if (options & (OPTION_FIX | OPTION_HEADER_REMOVE)) {
		string directory = path_to_pbo;
		size_t dot       = path_to_pbo.find_last_of(".");
		
		if (dot != string::npos)
			directory = path_to_pbo.substr(0, dot) + "\\";
			
		fstream file(path_to_pbo, ios::in | ios::binary);
		string contents;
	  
		if (file) {
			file.seekg(0, ios::end);
			contents.resize(file.tellg());
			file.seekg(0, ios::beg);
			file.read(&contents[0], contents.size());
			file.close();
		}
		
		bool save_file  = false;
		int files_start = 0;
		
		for (int i=0, file_count=0; i<contents.length(); ) {
			int j = i;
			
			while (contents[j] != '\0')
				j++;
				
			string name = contents.substr(i, j-i);
			i           = j + 1;
			
			unsigned int MimeType  = *((unsigned long*)&(contents.substr(i, 4))[0]);
			unsigned int TimeStamp = *((unsigned long*)&(contents.substr(i+12, 4))[0]);
			unsigned int Datasize  = *((unsigned long*)&(contents.substr(i+16, 4))[0]);
			i += 20;
			
			if (name.length() == 0) {
				if (file_count==0 && MimeType==MIME_PROPERTIES && TimeStamp==0 && Datasize==0) {
					int value_len = 0;
					bool is_name  = true;
					
					while (i < contents.length()) {
						if (contents[i++] != '\0')
							value_len++;
						else {
							if (is_name && value_len==0) {
								files_start = i;
								break;
							} else {
								is_name   = !is_name;
								value_len = 0;
							}
						}
					}
				} else
					break;
			} else
				if (options & OPTION_FIX) {
					string path_to_file = directory + name;
					
					WIN32_FILE_ATTRIBUTE_DATA fd;
					if (GetFileAttributesEx(path_to_file.c_str(), GetFileExInfoStandard, &fd)) {
						unsigned long file_stamp = (unsigned long)(FileTimeToUnixTime(fd.ftLastWriteTime));
	
						if (file_stamp != TimeStamp) {
							memcpy(&contents[0]+i-8, &file_stamp, 4);
							save_file = true;
						}						
					}
				}
			
			file_count++;
		}
		
		if (options & OPTION_HEADER_REMOVE  &&  files_start!=0) {
			contents  = contents.substr(files_start);
			save_file = true;
		}
		
	    if (save_file) {
			file.open(path_to_pbo, ios::out | ios::trunc | ios::binary);
			
			if (file.is_open()) {
				file << contents;
				file.close();
			}
			
			HANDLE file_handle = CreateFile(path_to_pbo.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			
			if (file_handle != INVALID_HANDLE_VALUE) {
				SetFileTime(file_handle, (LPFILETIME)NULL, (LPFILETIME)NULL, &fd.ftLastWriteTime);
				CloseHandle(file_handle);
			}
			
			GetFileAttributesEx(path_to_pbo.c_str(), GetFileExInfoStandard, &fd);
		}
	}
					
					
	// Read PBO file and store information about its entries in a record
	FILE *file = fopen(path_to_pbo.c_str(), "rb");
	if (!file) {
		cout << path_to_pbo << " - " << errno << " - ";
		perror("");
		return errno;
	}

	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	PBO_info_table record;
	PBO_single_entry file_properties;
	record.signature = "";
	record.origsize  = 0;
	record.datasize  = 0;
	record.timestamp = FileTimeToUnixTime(fd.ftLastWriteTime);
	bool first_entry = true;
	 
	while (ftell(file) < file_size) {
		string name = "";
		char c      = fgetc(file);
		
		while (c != '\0') {
			name += c;
			c     = fgetc(file);
		}
			
		fread(&file_properties, sizeof(file_properties), 1, file);
		record.datasize += file_properties.Datasize;
		
		if (file_properties.OriginalSize != 0)
			record.origsize += file_properties.OriginalSize;
		else
			record.origsize += file_properties.Datasize;
					
		if (name.length() == 0) {
			if (first_entry && file_properties.MimeType==MIME_PROPERTIES && file_properties.TimeStamp==0 && file_properties.Datasize==0) {
				bool is_value = false;
				
				while (ftell(file) < file_size) {
					char c = fgetc(file);
					
					if (c != '\0')
						name += c;
					else {
						if (!is_value && name.length()==0)
							break;
						else {
							record.properties.push_back(name);
							is_value = !is_value;
							name     = "";
						}
					}
				}
			} else {
				fseek(file, record.datasize, SEEK_CUR);
				
				while (ftell(file) < file_size)
					record.signature += fgetc(file);

				break;
			}
		} else {
			record.file_name.push_back(name);
			record.file_mime.push_back(file_properties.MimeType);
			record.file_origsize.push_back(file_properties.OriginalSize);
			record.file_time.push_back(file_properties.TimeStamp);
			record.file_datasize.push_back(file_properties.Datasize);
		}
		
		first_entry = false;
	}

	fclose(file);


	fstream GSscript;
	string GSscript_content = "";
	bool compare_Records    = false;
	
	if (options & OPTION_RECORD_SAVE) {
		options            &= ~OPTION_RECORD_SAVE;
		global_saved_record = record;
	} else 
		if (options & OPTION_RECORD_COMPARE) {
			options        &= ~OPTION_RECORD_COMPARE;
			compare_Records = true;
		}


	// Create log file and write information from the record
	fstream output;
	string output_name      = path_to_pbo.substr(0, path_to_pbo.find(".pbo"));
	ios_base::openmode mode = ios::out | ios::trunc;
	
	if (options & OPTION_SINGLELOG) {
		output_name = "PBOfilelist";
		
		if (options & OPTION_TRUNCATE)
			options &= ~OPTION_TRUNCATE;
		else {
			mode |= ios::app;
			mode &= ~ios::trunc;
		}
	}
		
 	output_name += (options & OPTION_CSV ? ".csv" : ".txt");
	
	output.open(output_name.c_str(), mode);
	
	if (output.is_open()) {
		if (options & OPTION_SINGLELOG  &&  ~mode & ios::trunc)
			output << endl << endl << "==================" << endl << endl;
		
		string separator = options & OPTION_CSV ? "," : " - ";
		
		if (~options & OPTION_CSV)
			output << "[";
			
		WIN32_FILE_ATTRIBUTE_DATA fd;
		GetFileAttributesEx(path_to_pbo.c_str(), GetFileExInfoStandard, &fd);
		
		output << path_to_pbo << separator << FormatDate(fd.ftLastWriteTime, options) << separator << FormatSize(fd.nFileSizeLow, options);
		
		if (~options & OPTION_CSV)
			output << "]";
			
		output << endl << endl;
		
		if (~options & OPTION_CSV  &&  record.properties.size() > 0) {
			output << "[Properties]" << endl;
			bool is_value = false;
			
			for (int i=0; i<record.properties.size(); i++) {
				output << record.properties[i];
				
				if (!is_value)
					output << "=";
				else
					output << endl;
	
				is_value = !is_value;
			}
			
			output << endl;
		}
		
		if (options & OPTION_CSV)
			output << "FILE NAME,LAST MODIFIED,FILE SIZE,COMPRESSED SIZE,MIME TYPE" << endl;
		
		if (record.file_name.size() > 0) {
			if (~options & OPTION_CSV) {
				output << "[" << record.file_name.size() << " file" << (record.file_name.size()!=1 ? "s" : "") << " - " << FormatSize(record.origsize, options);
				
				if (record.origsize != record.datasize)
					output << " (compressed to " << FormatSize(record.datasize, options) << ")";
				
				output << "]" << endl;
			}
			
			for (int i=0; i<record.file_name.size(); i++) {
				output << record.file_name[i] << separator << FormatDate(record.file_time[i], options) << separator;
				
				if (~options & OPTION_CSV) {
					if (record.file_origsize[i] != 0)
						output << FormatSize(record.file_origsize[i], options) << " (compressed to " << FormatSize(record.file_datasize[i], options) << ")";
					else
						output << FormatSize(record.file_datasize[i], options);
						
					switch(record.file_mime[i]) {
						case MIME_PROPERTIES : output << separator << "properties"; break;
						case MIME_ENCRYPTED  : output << separator << "encrypted"; break;
					}
				} else {
					if (record.file_origsize[i] != 0)
						output << FormatSize(record.file_origsize[i], options) << separator << FormatSize(record.file_datasize[i], options);
					else
						output << FormatSize(record.file_datasize[i], options) << separator << FormatSize(record.file_datasize[i], options);
						
					output << separator;
					
					switch(record.file_mime[i]) {
						case MIME_DUMMY      : output << "uncompressed"; break;
						case MIME_PROPERTIES : output << "properties"; break;
						case MIME_COMPRESSED : output << "compressed"; break;
						case MIME_ENCRYPTED  : output << "encrypted"; break;
					}
				}
								
				if (i < record.file_name.size()-1)
					output << endl;
					
				// Compare timestamps between two records
				if (compare_Records) {
					for (int j=0; j<global_saved_record.file_name.size(); j++)
						if (global_saved_record.file_name[j] == record.file_name[i]) {
							if (global_saved_record.file_time[j] != record.file_time[i]) {
								if (GSscript_content.empty())
									GSscript_content = "UNPBO \"" + path_to_pbo + "\"\n";
									
								GSscript_content += "FILEDATE \"" + global_saved_record.file_name[j] + "\" " + Int2Str(global_saved_record.file_time[j]) + "\n";
							}
							
							break;
						}
				}
			}
		}
		
		if (~options & OPTION_CSV  &&  !record.signature.empty())
			output << endl << endl << "[Signature]" << endl << record.signature;
		
		output.close();
		
		
		// Write a GS script after comparing records
		if (compare_Records  &&  !GSscript_content.empty()) {
			GSscript_content += "MAKEPBO\n" + (string)"FILEDATE \"" + path_to_pbo + "\" " + Int2Str(global_saved_record.timestamp);
			
			GSscript.open("PBOfilelist_GS.txt", ios::out | ios::trunc);
			if (GSscript.is_open()) {
				GSscript << GSscript_content;
				GSscript.close();
			}
		}
	}
	
	return 0;
}

int ScanDirectory(string path, int &options)
{
	WIN32_FIND_DATA fd;
	string wildcard  = path + "\\*";
	HANDLE hFind     = FindFirstFile(wildcard.c_str(), &fd);
	int return_value = 0;

	if (hFind == INVALID_HANDLE_VALUE)
		return FormatError(GetLastError());
	
	do {
		string file_name = (string)fd.cFileName;
		string full_path = path + "\\" + file_name;
		
		if (file_name == "." || file_name == "..")
			continue;
			
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (options & OPTION_RECURSIVE)
				return_value = ScanDirectory(full_path, options);
		} else {
			size_t dot = file_name.find_last_of('.');
			
			if (dot != string::npos) {
				string extension = file_name.substr(dot + 1);
				transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				
				if (extension == "pbo")
					return_value = CreatePBOLog(full_path, options);
			}
		}
	} while (FindNextFile(hFind, &fd) != 0);
	
	FindClose(hFind);
	return return_value;
}

int main(int argc, char *argv[]) 
{
	int return_value = 0;
	
	if (argc <= 1) {
		cout << "PBOfilelist v1.0 by Faguss (ofp-faguss.com)" << endl << endl
		<< "Usage: PBOfilelist [options] <file or dir> ..." << endl << endl
		<< "Options:" << endl
		<< "\t-t output timestamps instead of ISO8601 date" << endl
		<< "\t-b output full number of bytes instead of higher units" << endl
		<< "\t-c output csv file instead of txt" << endl
		<< "\t-f correct timestamps in the PBO (if source folder exists)" << endl
		<< "\t-r scan directories recursively (when filename is dir name)" << endl
		<< "\t-s output to a single file for all PBOs instead of different file for each PBO" << endl
		<< "\t-g compare timestamps between the first and second PBO and output script for GS website" << endl
		<< "\t-h remove properties header from the PBO" << endl;
	} else {
		int options = OPTION_NONE;
		
		for(int i=1; i<argc; i++) {
			if (argv[i][0] == '-') {
				int len = strlen(argv[i]);
				
				for (int j=1; j<len; j++) {
					switch(argv[i][j]) {
						case 'T' : 
						case 't' : options |= OPTION_TIMESTAMP; break;
						case 'B' :
						case 'b' : options |= OPTION_FULLBYTES; break;
						case 'C' :
						case 'c' : options |= OPTION_CSV; break;
						case 'F' :
						case 'f' : options |= OPTION_FIX; break;
						case 'R' :
						case 'r' : options |= OPTION_RECURSIVE; break;
						case 'S' :
						case 's' : options |= OPTION_SINGLELOG | OPTION_TRUNCATE; break;
						case 'G' : 
						case 'g' : options |= OPTION_RECORD_SAVE | OPTION_RECORD_COMPARE; break;
						case 'H' :
						case 'h' : options |= OPTION_HEADER_REMOVE; break;
					}
				}
			} else {
				DWORD attributes = GetFileAttributes(argv[i]);
				
				if (attributes!=INVALID_FILE_ATTRIBUTES && attributes & FILE_ATTRIBUTE_DIRECTORY) {
					return_value = ScanDirectory(argv[i], options);
				} else					
					return_value = CreatePBOLog(argv[i], options);
			}
		}
	}

	return return_value;
}
