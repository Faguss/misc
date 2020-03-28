#include <iostream>
#include <windows.h>
#include <vector>
#include <fstream>

using namespace std;

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

	string ret = "   - " + (string)(char*)errorText + "\n";

	if (errorText != NULL)
		LocalFree(errorText);

	return ret;
}


int list_directory(string path, vector<string> &container) {
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;
	string pattern = path + "\\*";
	hFile          = FindFirstFile(pattern.c_str(), &FileInformation);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		int errorCode = GetLastError();
		cout << "Failed to list files in " << path << " - " << errorCode << " " << FormatError(errorCode);
		return 1;
	}
	
	do {
		string file_name = FileInformation.cFileName;
		
		if (file_name == "." || file_name == "..")
			continue;

		container.push_back(file_name);
	} 
	while(FindNextFile(hFile, &FileInformation) == TRUE);
	
	FindClose(hFile);
	return 0;
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


string GetIsland(string file_name, string &mission_name)
{
	string file_extension = file_name.substr( file_name.find_last_of('.')+1 );
	
	if (Equals(file_extension,"pbo")) {
		size_t lastDot       = file_name.find_last_of('.');
		size_t secondLastDot = file_name.find_last_of('.', lastDot-1);
		
		if (lastDot!=string::npos  &&  secondLastDot!=string::npos) {
			file_extension = file_name.substr( secondLastDot+1, lastDot-secondLastDot-1 );
			mission_name   = file_name.substr( 0, secondLastDot );
		}	
	} else {
		mission_name = file_name.substr( 0, file_name.find_last_of('.') );
	}

	return file_extension;
}

	// http://stackoverflow.com/a/3418285
string ReplaceAll(string str, const string& from, const string& to) 
{
    if (from.empty())
        return str;
        
    size_t start_pos = 0;
    
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    
    return str;
}

	// Remove quotation marks
string UnQuote(string text)
{
	if (text.substr(text.length()-1) == "\"")
		text = text.substr(0, text.length()-1);
	
	if (text.substr(0,1) == "\"")
		text = text.substr(1);
		
	return text;	
}


void Tokenize(string text, string delimiter, vector<string> &container)
{
	bool first_item = false;
	bool inQuote    = false;
	
	// Split line into parts
	for (int pos=0, begin=-1;  pos<=text.length();  pos++) {
		bool isToken = pos == text.length();
		
		for (int i=0;  !isToken && i<delimiter.length();  i++)
			if (text.substr(pos,1) == delimiter.substr(i,1))
				isToken = true;
				
		if (text.substr(pos,1) == "\"")
			inQuote = !inQuote;
			
		// Mark beginning of the word
		if (!isToken  &&  begin<0)
			begin = pos;
						
		// Mark end of the word
		if (isToken  &&  begin>=0  &&  !inQuote) {
			string part = UnQuote(text.substr(begin, pos-begin));
			container.push_back(part);
			begin = -1;
		}
	}
}


string simplify_mission_name(string str) {
	GetIsland(str, str);

	str = ReplaceAll(str, "'", "");
	
	vector<string> words;
	Tokenize(str, " _-0123456789@&=[].", words);
	
	if (words.size() == 0)
		return str;
		
	vector<string> words_to_ignore;
	words_to_ignore.push_back("ctrcti");
	words_to_ignore.push_back("wwii");
	words_to_ignore.push_back("fdf");
	words_to_ignore.push_back("fdfd");
	words_to_ignore.push_back("fdf13");
	words_to_ignore.push_back("fdf14");
	words_to_ignore.push_back("fdfbas");
	words_to_ignore.push_back("mfcti");
	words_to_ignore.push_back("coop");
	words_to_ignore.push_back("pmc");
	words_to_ignore.push_back("tdm");
	words_to_ignore.push_back("len");
	words_to_ignore.push_back("cti");
	words_to_ignore.push_back("crcti");
	words_to_ignore.push_back("ctf");
	words_to_ignore.push_back("the");
	words_to_ignore.push_back("bas");
	words_to_ignore.push_back("REVIVE");
	
	for (int i=0; i<words.size(); i++) {
		if (words[i].length() < 3)
			continue;
			
		bool ignore = false;
		for(int j=0; j<words_to_ignore.size(); j++)
			if (Equals(words[i],words_to_ignore[j])) {
				ignore = true;
				break;
			}
			
		if (!ignore)
			return words[i];
	}

	return words[0];
}






int main()
{	
	const int max_islands = 50;
	vector<string> mission_list;
	vector<string> island_list;
	vector<string> mission_island_list[max_islands];
	list_directory("mpmissions", mission_list);
	
	for (int i=0; i<mission_list.size(); i++) {
		string mission_name = mission_list[i];
		string island       = GetIsland(mission_list[i], mission_name);
		
		int j             = 0;
		bool found_island = false;
		for (; j<island_list.size(); j++) {
			if (Equals(island,island_list[j])) {
				found_island = true;
				break;
			}
		}
		
		
		if (j < max_islands) {
			if (!found_island)
				island_list.push_back(island);
			
			mission_island_list[j].push_back(mission_list[i]);
		} else
			cout << "out of space" << endl;
		
	}
	
				
	fstream file;
	file.open("list_ofp_missions.txt", ios::out | ios::trunc);
	if (file.is_open()) {
		for (int i=0; i<island_list.size(); i++) {
			file << island_list[i] << endl;
			
			for (int j=0; j<mission_island_list[i].size(); j++) {			
				string mission = mission_island_list[i][j];
				
				if (mission == "")
					continue;
				
				string mission_without_island = mission;
				GetIsland(mission, mission_without_island);
				
				string simple = simplify_mission_name(mission);
				file << "\t" << mission_without_island << "   - " << simple << endl;
				
				
				for (int k=0; k<mission_island_list[i].size(); k++) {
					string match_against        = mission_island_list[i][k];
					string simple_match_against = simplify_mission_name(match_against);
					
					if (j==k || match_against == "")
						continue;
						
					if (Equals(simple,simple_match_against)) {
						string match_against_without_island = match_against;
						GetIsland(match_against, match_against_without_island);
						file << "\t" << match_against_without_island << endl;
						mission_island_list[i][k] = "";
					}
				}
				
				file << endl;
			}
				
			file << "===========================" << endl << endl << endl << endl << endl << endl;
		}

		file << "Mission count: " << mission_list.size() << endl << "Island count: " << island_list.size() << endl;
		file.close();
	} else
		cout << "Failed to write file" << endl;
		
	//system("pause");
	return 0;
}
