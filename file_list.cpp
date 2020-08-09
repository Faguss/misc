// Generate text file listing all the files in the current directory

#include <sstream>
#include <windows.h>
#include <fstream>
#include <vector>
using namespace std;

//https://stackoverflow.com/questions/6691555/converting-narrow-string-to-wide-string
wstring string2wide(const string& input)
{
    if (input.empty())
		return wstring();

	size_t output_length = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), 0, 0);
	wstring output(output_length, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), &output[0], (int)input.length());
	
	return output;
}

//https://mariusbancila.ro/blog/2008/10/20/writing-utf-8-files-in-c/
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



void browse_directory(wstring input_path, wstring input_pattern, ofstream &output) 
{
	WIN32_FIND_DATAW FileInformation;
	wstring pattern = input_path + (!input_path.empty() ? L"\\" : L"") + input_pattern;
	HANDLE hFile    = FindFirstFileW(pattern.c_str(), &FileInformation);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		int errorCode = GetLastError();
		output << "Failed to list files in " << wide2string(input_path) << " - " << FormatError(errorCode) << endl;
		return;
	}
	
	int number_of_files = 0;
	
	do {
		wstring current_file = (wstring)FileInformation.cFileName;
		
		if (current_file != L"."  &&  current_file != L"..") {
			number_of_files++;
			wstring path_to_current_file = input_path + (!input_path.empty() ? L"\\" : L"") + current_file;

			if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				browse_directory(path_to_current_file, input_pattern, output);
			else
				output << wide2string(path_to_current_file) << endl;
		}
	}
	while(FindNextFileW(hFile, &FileInformation));
	
	// If empty directory then include its name
	if (number_of_files==0 && !input_path.empty())
		output << wide2string(input_path) << "\\" << endl;
	
	FindClose(hFile);
	return;
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
	
	
	ofstream logfile;
	logfile.open("file_list.txt", ios::out | ios::trunc);
	browse_directory(string2wide(input_path), string2wide(input_pattern), logfile);
	logfile.close();
	return 0;
}
