#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <wchar.h>

std::wstring current_file   = L"";
time_t current_highest      = 0;
FILETIME current_highest_ft = {0};

int ScanDirectory(std::wstring path)
{
	WIN32_FIND_DATAW fd;
	std::wstring wildcard = path + L"\\*";
	HANDLE hFind          = FindFirstFileW(wildcard.c_str(), &fd);
	int return_value      = 0;

	if (hFind == INVALID_HANDLE_VALUE)
		return 2;
	
	do {
		std::wstring file_name = (std::wstring)fd.cFileName;
		std::wstring full_path = path + L"\\" + file_name;
		
		if (file_name == L"." || file_name == L"..")
			continue;
			
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			ScanDirectory(full_path);
		} else {
			ULARGE_INTEGER ull;
			ull.LowPart  = fd.ftLastWriteTime.dwLowDateTime;
			ull.HighPart = fd.ftLastWriteTime.dwHighDateTime;
			time_t stamp = ull.QuadPart / 10000000ULL - 11644473600ULL;
			
			if (stamp > current_highest && file_name!=L"getLatestDate.exe") {
				current_highest    = stamp;
				current_file       = full_path;
				current_highest_ft = fd.ftLastWriteTime;
			}
		}
	} while (FindNextFileW(hFind, &fd) != 0);
	
	FindClose(hFind);
	return 0;
}

std::wstring Int2StrW(int num)
{
	const int buffer_size       = 16;
	wchar_t buffer[buffer_size] = L"";
	
	//swprintf_s(buffer_ptr, buffer_size, L"%s%d", (leading_zero && num < 10 ? L"0" : L""), num);
	_swprintf(buffer, L"%s%d", (num < 10 ? L"0" : L""), num);
	return (std::wstring)buffer;
}
	
int wmain(int argc, wchar_t *argv[])
{
	std::wstring path = L".";
	
	if (argc >= 2)
		path = (std::wstring)argv[1];
		
	ScanDirectory(path);
	SYSTEMTIME st = {0};
	FileTimeToSystemTime(&current_highest_ft, &st);
	
	std::wcout << 
		current_file << L" - " <<
		Int2StrW(st.wYear) << L"." << 
		Int2StrW(st.wMonth) << L"." << 
		Int2StrW(st.wDay) << L" " << 
		Int2StrW(st.wHour) << L":" << 
		Int2StrW(st.wMinute) << L":" << 
		Int2StrW(st.wSecond) << std::endl;
	
	return 0;
}
