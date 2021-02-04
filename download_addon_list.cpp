// Downloads web pages with wget 
// from certain domains containing addon list for OFP
// reads them and outputs SQL addon list
// There's a functon for each domain; will turn into a table walk later

#include <iostream>
#include <fstream>		// file operations
#include <windows.h>	// winapi
#include <tlhelp32.h>	// process/module traversing
#include <unistd.h>     // for access command
#include <vector>       // dynamic array
#include <algorithm>	// tolower
#include <sstream>      // for converting int to string
#include <Shlobj.h>		// opening explorer
#include <map>			// associative array for arguments
#include <time.h>		// get current time as unix timestamp

using namespace std;


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

string HandleQuotes(string str, const string& from, const string& to) 
{
	if (from.empty())
        return str;
        
    size_t start_pos = 0;
    
    while ((start_pos = str.find(from, start_pos)) != string::npos) {		
		if (start_pos==0 || str.substr(start_pos-1,1) != "\\") {
			str.replace(start_pos, from.length(), to);
		}
		
        start_pos += to.length();
    }
    
    return str;
}

string Int2Str(int num)
{
    ostringstream text;
    text << num;
    return text.str();
}

string Trim(string s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

	// Windows error message
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

	return Trim(ret);
}

int Download(string url, string filename, bool overwrite=true) 
{
	if (overwrite)
		DeleteFile(filename.c_str());
	
	cout << url << endl << endl;
	string arguments   = " --tries=3 --no-check-certificate --no-clobber --remote-encoding=utf-8 --output-document=" + filename + " " + url;
	
	// Execute program
	PROCESS_INFORMATION pi;
    STARTUPINFO si; 
	ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &pi, sizeof(pi) );
	si.cb 		   = sizeof(si);
	si.dwFlags 	   = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	
	if (CreateProcess("wget.exe", &arguments[0], NULL, NULL, false, 0, NULL, NULL, &si, &pi)) {
		DWORD exit_code;	
		Sleep(10);
	
		do {
			GetExitCodeProcess(pi.hProcess, &exit_code);
			Sleep(100);
		}
		while (exit_code == STILL_ACTIVE);
			
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	} else {
		int errorCode = GetLastError();
		cout << FormatError(errorCode);
		return errorCode;
	}
	
	Sleep(1000);
	return 0;
}


int Read(string filename, string &buffer) {
	buffer = "";		
	fstream file;
	file.open(filename.c_str(), ios::in);
	
	if (file.is_open()) {
		string line;
		
		while (getline(file, line))
			buffer += line;
		
		file.close();
	} else
		return errno;
		
	return 0;	
}

int Get(string url, string filename, string &buffer, bool overwrite=true)
{
	Download(url, filename, overwrite);
	return Read(filename, buffer);
}

enum GET_OPTIONS {
	SKIP_IF_EXISTS,
	REDOWNLOAD
};

enum SEARCH_ORDER {
	NORMAL,
	REVERSE
};

string GetTextBetween(string &buffer, string start, string end, size_t &offset, bool reverse=false)
{
	string out  = "";
	size_t pos0 = buffer.find(start, offset);
	
	if (!reverse) {
		if (pos0 != string::npos) {
			size_t pos1 = pos0 + start.length();
			size_t pos2 = buffer.find(end, pos1);
			
			if (pos2 != string::npos) {
				offset = pos1;
				out    = buffer.substr(pos1, pos2-pos1);
			}
		}		
	} else {
		if (pos0 != string::npos) {
			size_t pos1 = buffer.rfind(end, pos0);			
			
			if (pos1 != string::npos) {
				offset      = pos0 + start.length();
				size_t pos2 = pos1 + end.length();
				out         = buffer.substr(pos2, pos0-pos2);
			}
		}			
	}

	return out;
}

enum ITEM_PROPERTIES {
	TITLE,
	FILENAME,
	FILESIZE,
	DATETIME,
	DOWNLOADS,
	DESCRIPTION
};

int DSServers()
{
	string base_folder = "DSServers";
	string base_url    = "https://ds-servers.com";
	
	CreateDirectory(base_folder.c_str(), NULL);
	
	vector<string> games_to_explore;
		games_to_explore.push_back("operation-flashpoint");
		games_to_explore.push_back("operation-flashpoint-resistance");
	
	string page_buffer;
	string item_buffer;
	string page_file_name;
	string item_file_name;

	int item_id = 0;
	
	for(int i=0; i<games_to_explore.size(); i++) {
		string game_folder = base_folder + "\\" + games_to_explore[i];
		
		if (GetFileAttributes(game_folder.c_str()) != INVALID_FILE_ATTRIBUTES)
			continue;
		
		CreateDirectory(games_to_explore[i].c_str(), NULL);
		
		int current_page   = 1;
		string initial_url = base_url + "/gf/" + games_to_explore[i] + "/";
		page_file_name     = game_folder + "\\page" + Int2Str(current_page) + ".htm";
		
		Get(initial_url, page_file_name, page_buffer);
		
		size_t offset = 0;
		string max_page_str = GetTextBetween(page_buffer, "... <a href=\"/gf/" + games_to_explore[i] + "/", "\"", offset);
		
		int max_page = atoi(max_page_str.c_str());
		cout << "max_page: " << max_page << endl;
		
		while (current_page <= max_page) {			
			if (current_page != 1) {
				string page_url = initial_url + Int2Str(current_page);
				page_file_name  = game_folder + "\\page" + Int2Str(current_page) + ".htm";
				cout << "page_url:" << page_url << endl;
				Get(page_url, page_file_name , page_buffer);
			}
			
			int total_items = 0;
			offset = 0;
			string item_link = "";
			string rel_link  = "";
			
			do {
				rel_link  = GetTextBetween(page_buffer, "\" rel=\"nofollow\" itemprop=\"url\">", "<a href=\"", offset, REVERSE);
				item_link = base_url + rel_link;
				cout << "item_link:" << item_link << endl;
				
				if (rel_link.empty()) {
					cout << "EMPTY PAGE " << total_items << endl;
					break;
				}
				
				total_items++;
				
				item_file_name = game_folder + "\\item" + Int2Str(item_id) + ".htm";
				Get(item_link, item_file_name, item_buffer);
			
				item_id++;
			} while (!rel_link.empty());
			
			current_page++;
		}
	}



	vector<string> item_properties;
	vector<string> item_properties_html;
		item_properties_html.push_back("<h1 style=\"text-align: left;margin: 0;\">");
		item_properties_html.push_back("</h1>");
		
		item_properties_html.push_back("<dt>File Name:</dt><dd>");
		item_properties_html.push_back("</dd>");
		
		item_properties_html.push_back("<dt>File Size:</dt><dd>");
		item_properties_html.push_back("</dd>");
		
		item_properties_html.push_back("</div><div style=\"display:inline;float:right\">");
		item_properties_html.push_back("</div>");
		
		item_properties_html.push_back("Downloads All Time:</dt><dd>");
		item_properties_html.push_back("</dd>");
		
		item_properties_html.push_back("<div class=\"row\" style=\"margin: 20px 0;\"><div class=\"column width-6\">");
		item_properties_html.push_back("</div>");
		
	fstream output;
	output.open(base_folder+".sql", ios::out | ios::trunc);
	output << "CREATE TABLE `" << base_folder << "` ("
  "`ID` int(11) NOT NULL,"
  "`Title` text NOT NULL,"
  "`File` text NOT NULL,"
  "`Game` text NOT NULL,"
  "`Path` text NOT NULL,"
  "`Permalink` text NOT NULL,"
  "`Size` text NOT NULL,"
  "`Date` date NOT NULL,"
  "`Downloads` int(11) NOT NULL,"
  "`Description` text NOT NULL"
") ENGINE=MyISAM DEFAULT CHARSET=utf8;" << endl << 
"INSERT INTO `" << base_folder << "` (`ID`, `Title`, `File`, `Game`, `Path`, `Permalink`,`Size`, `Date`, `Downloads`, `Description`) VALUES" << endl;

	int game_id = 0;
	
	for (int i=0; game_id<games_to_explore.size(); i++) {
		string item_file_name = base_folder + "\\" + games_to_explore[game_id] + "\\item" + Int2Str(i) + ".htm";
		cout << item_file_name << endl;
		
		if (GetFileAttributes(item_file_name.c_str()) == INVALID_FILE_ATTRIBUTES) {
			game_id++;
			
			if (game_id<games_to_explore.size()) {
				cout << "next game" << endl;
				i--;
				continue;
			} else {
				output << ";";
				cout << "end loop" << endl;
				break;
			}
		}
		
		Read(item_file_name, item_buffer);

		if (item_buffer.empty()) {
			cout << "EMPTY BUFFER" << endl;
			break;
		}
		
		item_properties.clear();
																			
		for (int j=0; j<item_properties_html.size()-1; j+=2) {
			size_t tmp = 0;
			string item_property = GetTextBetween(item_buffer, item_properties_html[j], item_properties_html[j+1], tmp);
			
			if (item_property == "") {
				cout << "ERROR: Couldn't find " << item_properties_html[j] << endl;
				break;
			}
			
			item_properties.push_back(HandleQuotes(item_property, "'", "\\'"));
		}
		
		string initial_url = base_url + "/gf/" + games_to_explore[game_id] + "/";
		
		size_t tmp_offset  = 0;
		string item_link   = GetTextBetween(item_buffer, "<link rel=\"alternate\" hreflang=\"ru\" href=\"", "\" />", tmp_offset);
		
		size_t last_slash  = item_link.find_last_of("/");
		string permalink   = item_link.substr(last_slash+1);
		string path        = item_link.substr(initial_url.length(), last_slash-initial_url.length());

		if (i != 0)
			output << ",";

		output 
		<< "(" 
		<< i 
		<< ",'" 
		<< item_properties[TITLE] 
		<< "','" 
		<< item_properties[FILENAME] 
		<< "','" 
		<< games_to_explore[game_id] 
		<< "','" 
		<< path 
		<< "','"
		<< permalink
		<< "','"
		<< item_properties[FILESIZE] 
		<< "','" 
		<< item_properties[DATETIME].substr(0,10)
		<< "'," 
		<< ReplaceAll(item_properties[DOWNLOADS] , ",", "") 
		<< ",'" 
		<< item_properties[DESCRIPTION]
		<< "')" 
		<< endl;
	}
	
	output.close();
	return 0;
}

struct HtmlFile {
	string buffer;
	string file_name;
	size_t search_offset;
	vector<string> found_strings;
};

int HtmlFile_Get(HtmlFile &record, string url, bool overwrite=true)
{			
	record.search_offset = 0;
	
	if (overwrite || GetFileAttributes(record.file_name.c_str()) == INVALID_FILE_ATTRIBUTES)
		Download(url, record.file_name, overwrite);

	return Read(record.file_name, record.buffer);
}

void HtmlFile_Search(HtmlFile &record, string start, string end, bool reverse=false)
{
	record.found_strings.clear();
	string text = "";
	
	do {
		text = GetTextBetween(record.buffer, start, end, record.search_offset, reverse);
		
		if (!text.empty())
			record.found_strings.push_back(text);
	} while (!text.empty());
}

int LoneBullet() {
	string base_folder = "LoneBullet";
	string base_url    = "https://www.lonebullet.com";
	
	vector<string> games_to_explore;
		games_to_explore.push_back("operation-flashpoint-4422.htm");
		games_to_explore.push_back("operation-flashpoint-resistance-4419.htm");
		games_to_explore.push_back("arma-cold-war-assault-7988.htm");
	
	CreateDirectory(base_folder.c_str(), NULL);
	
	for(int i=0; i<games_to_explore.size(); i++) {
		string game_folder = base_folder + "\\" + games_to_explore[i];
		
		if (GetFileAttributes(game_folder.c_str()) != INVALID_FILE_ATTRIBUTES)
			continue;
		
		CreateDirectory(game_folder.c_str(), NULL);
		
		string main_page_url = base_url + "/games/" + games_to_explore[i];
		
		HtmlFile main_page;
		main_page.file_name = game_folder + "\\main.htm";
		HtmlFile_Get(main_page, main_page_url, SKIP_IF_EXISTS);
		
		size_t tmp_offset = 0;
		main_page.buffer = GetTextBetween(main_page.buffer, "<h2><img class='h2img' src='/imgs/downloadsimg.png' />Downloads</h2>", "</div>", tmp_offset);
		HtmlFile_Search(main_page, "<a href='", "'");
		
		int item_num = 0;

		for (int i=0; i<main_page.found_strings.size(); i++) {
			int page_num        = 0;
			bool next_page      = true;
			string query_string = "";
			
			while(next_page) {
				HtmlFile section_page;
				section_page.file_name = game_folder + "\\section" + Int2Str(i) + "page" + Int2Str(page_num) + ".htm";
				string section_url     = base_url + main_page.found_strings[i] + query_string;
				HtmlFile_Get(section_page, section_url, SKIP_IF_EXISTS);
				
				tmp_offset          = 0;
				section_page.buffer = GetTextBetween(section_page.buffer, "<div class='wrapper'>", "<div id='sidebar'>", tmp_offset);
				
				HtmlFile_Search(section_page, "<span class='clear'><div class='name' style='float:left'><a href='", "'");
				
				for (int j=0; j<section_page.found_strings.size(); j++) {
					HtmlFile item_page;
					item_page.file_name = game_folder + "\\item" + Int2Str(item_num) + ".htm";
					string item_url     = base_url + section_page.found_strings[j];
					HtmlFile_Get(item_page, item_url, SKIP_IF_EXISTS);
					HtmlFile_Search(item_page, "'><img src='/imgs/downloadbtn.png'", "<a href='", REVERSE);
					
					if (item_page.found_strings.size() > 0) {
						HtmlFile iteminfo_page;
						iteminfo_page.file_name = game_folder + "\\iteminfo" + Int2Str(item_num) + ".htm";
						string iteminfo_url     = base_url + item_page.found_strings[0];
						HtmlFile_Get(iteminfo_page, iteminfo_url, SKIP_IF_EXISTS);
					}

					item_num++;
				}
				
				tmp_offset = 0;
				query_string = GetTextBetween(section_page.buffer, "'><div class=pageselecter>Next", "href='", tmp_offset, REVERSE);
				
				if (query_string.empty())
					next_page = false;
				else					
					page_num++;
			}
		}
	}
	
	
	

	enum LB_ITEM_PROPERTIES {
		LB_TITLE,
		LB_FILENAME,
		LB_FILESIZE,
		LB_DOWNLOADS,
		LB_DESCRIPTION
	};

	vector<string> item_properties;
	vector<string> item_properties_html;
		item_properties_html.push_back("<h1>");
		item_properties_html.push_back("</h1>");
		
		item_properties_html.push_back("<font style='color: #F59D28'>File Name:</font> ");
		item_properties_html.push_back("<br />");
		
		item_properties_html.push_back("<font style='color: #F59D28'>Size:</font> ");
		item_properties_html.push_back("<br />");
			
		item_properties_html.push_back("<font style='color: #F59D28'>Downloads:</font> ");
		item_properties_html.push_back("<br />");
		
		item_properties_html.push_back("<span class='desch'>Description</span><br />");
		item_properties_html.push_back("</div>");
			
	string item_buffer = "";
	string iteminfo_buffer = "";
	int record_id = 0;
	
	fstream output;
	output.open(base_folder+".sql", ios::out | ios::trunc);
	output << "CREATE TABLE `" << base_folder << "` ("
  "`ID` int(11) NOT NULL,"
  "`Title` text NOT NULL,"
  "`File` text NOT NULL,"
  "`Permalink` text NOT NULL,"
  "`Size` text NOT NULL,"
  "`Downloads` int(11) NOT NULL,"
  "`Description` text NOT NULL"
") ENGINE=MyISAM DEFAULT CHARSET=utf8;" << endl <<
"INSERT INTO `" << base_folder << "` (`ID`, `Title`, `File`, `Permalink`,`Size`, `Downloads`, `Description`) VALUES" << endl;

	for (int i=0; i<games_to_explore.size(); i++) {
		int item_id = 0;
		
		do {
			string item_file_name      = base_folder + "\\" + games_to_explore[i] + "\\item" + Int2Str(item_id) + ".htm";
			string item_info_file_name = base_folder + "\\" + games_to_explore[i] + "\\iteminfo" + Int2Str(item_id) + ".htm";
			cout << item_file_name << endl;
			
			if (GetFileAttributes(item_file_name.c_str()) == INVALID_FILE_ATTRIBUTES) {
				break;
			}
			
			Read(item_file_name, item_buffer);
			Read(item_info_file_name, iteminfo_buffer);
			item_properties.clear();
			
			for (int j=0; j<item_properties_html.size()-1; j+=2) {
				size_t tmp = 0;
				string item_property = "";
				
				if (j/2 == LB_TITLE || j/2 == LB_DESCRIPTION)
					item_property = GetTextBetween(item_buffer, item_properties_html[j], item_properties_html[j+1], tmp);
				else
					item_property = GetTextBetween(iteminfo_buffer, item_properties_html[j], item_properties_html[j+1], tmp);

				if (item_property == "" && j/2 != LB_DESCRIPTION) {
					cout << "ERROR: Couldn't find " << item_properties_html[j] << endl;
					break;
				}
				
				if (j/2 == LB_DESCRIPTION) {					
					for (int k=0; k<item_property.length(); k++) {						
						if (item_property[k] == 0xFFFFFF92)
							item_property[k] = '\'';
							
						if (!isprint(item_property[k]))
							item_property[k] = ' ';
					}
				}
							
				item_properties.push_back(HandleQuotes(item_property, "'", "\\'"));
			}
			
			size_t tmp_offset = 0;
			string permalink  = GetTextBetween(iteminfo_buffer, "' style='text-decoration:none'><img style=\"vertical-align:middle;\" src='/imgs/Back.png' />", "<a href='", tmp_offset, REVERSE);
			
			if (record_id != 0)
				output << "," << endl;
			
			output 
			<< "(" 
			<< record_id
			<< ",'" 
			<< item_properties[LB_TITLE] 
			<< "','" 
			<< item_properties[LB_FILENAME] 
			<< "','" 
			<< permalink
			<< "','"
			<< item_properties[LB_FILESIZE] 
			<< "'," 
			<< ReplaceAll(item_properties[LB_DOWNLOADS] , ",", "") 
			<< ",'" 
			<< item_properties[LB_DESCRIPTION]
			<< "')" ;
			
			record_id++;
			item_id++;
		} while(true);
	}
	
	output << ";";
	
	output.close();
}

int ModDB() 
{
	string base_folder = "ModDB";
	string base_url    = "https://www.moddb.com";
	
	vector<string> games_to_explore;
		games_to_explore.push_back("operation-flashpoint");
		games_to_explore.push_back("arma-cold-war-assault");	
		
	CreateDirectory(base_folder.c_str(), NULL);
	
	for(int i=0; i<games_to_explore.size(); i++) {
		string game_folder = base_folder + "\\" + games_to_explore[i];
		
		if (GetFileAttributes(game_folder.c_str()) != INVALID_FILE_ATTRIBUTES)
			continue;
		
		CreateDirectory(game_folder.c_str(), NULL);
		
		bool next_page = true;
		int page_num   = 0;
		int item_num   = 0;
		
		while(next_page) {
			HtmlFile current_page;
			current_page.file_name = game_folder + "\\page" + Int2Str(page_num) + ".htm";
			string page_url        = base_url + "/games/" + games_to_explore[i] + "/downloads";
			
			if (page_num !=0)
				page_url += "/page/" + Int2Str(page_num+1);
			
			HtmlFile_Get(current_page, page_url, SKIP_IF_EXISTS);
			
			size_t tmp_offset   = 0;
			current_page.buffer = GetTextBetween(current_page.buffer, "<div class=\"headernormalbox normalbox\">", "<div class=\"nextmediabox nextmediaboxtop\">", tmp_offset);
			
			HtmlFile_Search(current_page, "<h4><a href=\"", "\">");
			
			for (int j=0; j<current_page.found_strings.size(); j++) {
				HtmlFile item_page;
				item_page.file_name = game_folder + "\\item" + Int2Str(item_num) + ".htm";
				string item_url     = base_url + current_page.found_strings[j];
				HtmlFile_Get(item_page, item_url, SKIP_IF_EXISTS);
				item_num++;
			}
			
			tmp_offset = 0;
			if (GetTextBetween(current_page.buffer, "\" class=\"next\" title=\"Go to the next page\">", "<a href=\"", tmp_offset, REVERSE).empty())
				next_page = false;
			else					
				page_num++;
		}
	}

	

	vector<string> item_properties;
	vector<string> item_properties_html;
		item_properties_html.push_back("<span class=\"heading\">");
		item_properties_html.push_back("</span>");
		
		item_properties_html.push_back("<h5>Filename</h5>						<span class=\"summary\">							");
		item_properties_html.push_back("</span>");
		
		item_properties_html.push_back("<h5>Size</h5>						<span class=\"summary\">							");
		item_properties_html.push_back(" (");
			
		item_properties_html.push_back("<h5>Added</h5>						<span class=\"summary\">							<time datetime=\"");
		item_properties_html.push_back("\"");
		
		item_properties_html.push_back(" Statistics\">");
		item_properties_html.push_back(" (");
		
		item_properties_html.push_back("<p id=\"downloadsummary\">");
		item_properties_html.push_back("</div>");
			
	string item_buffer = "";
	string iteminfo_buffer = "";
	int record_id = 0;
	
	fstream output;
	output.open(base_folder+".sql", ios::out | ios::trunc);
	output << "CREATE TABLE `" << base_folder << "` ("
  "`ID` int(11) NOT NULL,"
  "`Title` text NOT NULL,"
  "`File` text NOT NULL,"
  "`Permalink` text NOT NULL,"
  "`Size` text NOT NULL,"
  "`Date` date NOT NULL,"
  "`Downloads` int(11) NOT NULL,"
  "`Description` text NOT NULL"
") ENGINE=MyISAM DEFAULT CHARSET=utf8;" << endl << 
"INSERT INTO `" << base_folder << "` (`ID`, `Title`, `File`, `Permalink`,`Size`, `Date`, `Downloads`, `Description`) VALUES" << endl;

	for (int i=0; i<games_to_explore.size(); i++) {
		int item_id = 0;
		
		do {
			string item_file_name      = base_folder + "\\" + games_to_explore[i] + "\\item" + Int2Str(item_id) + ".htm";
			cout << item_file_name << endl;
			
			if (GetFileAttributes(item_file_name.c_str()) == INVALID_FILE_ATTRIBUTES) {
				break;
			}
			
			Read(item_file_name, item_buffer);
			item_properties.clear();
			
			for (int j=0; j<item_properties_html.size()-1; j+=2) {
				size_t tmp = 0;
				string item_property = GetTextBetween(item_buffer, item_properties_html[j], item_properties_html[j+1], tmp);							
				item_properties.push_back(Trim(HandleQuotes(item_property, "'", "\\'")));
			}
			
			size_t tmp_offset = 0;
			string permalink  = GetTextBetween(item_buffer, "<input type=\"hidden\" name=\"referer\" value=\"", "\"", tmp_offset);
			
			if (record_id != 0)
				output << "," << endl;
			
			output 
			<< "(" 
			<< record_id 
			<< ",'" 
			<< item_properties[TITLE] 
			<< "','" 
			<< item_properties[FILENAME] 
			<< "','" 
			<< permalink
			<< "','"
			<< item_properties[FILESIZE] 
			<< "','" 
			<< item_properties[DATETIME].substr(0,10)
			<< "'," 
			<< ReplaceAll(item_properties[DOWNLOADS] , ",", "") 
			<< ",'" 
			<< item_properties[DESCRIPTION]
			<< "')";
			
			record_id++;
			item_id++;
		} while(true);
	}
	
	output << ";";
	
	output.close();
}

void BIForum() 
{
	string base_folder = "BIForum";
	string base_url    = "https://forums.bohemia.net";
	
	vector<string> games_to_explore;
		games_to_explore.push_back("22-addons-amp-mods-complete");
		games_to_explore.push_back("52-addons-amp-mods-discussion");
		
	CreateDirectory(base_folder.c_str(), NULL);
	
	for(int i=0; i<games_to_explore.size(); i++) {
		string game_folder = base_folder + "\\" + games_to_explore[i];
		
		if (GetFileAttributes(game_folder.c_str()) != INVALID_FILE_ATTRIBUTES)
			continue;
		
		CreateDirectory(game_folder.c_str(), NULL);
		
		bool next_page = true;
		int page_num   = 0;
		int item_num   = 0;
		
		while(next_page) {
			HtmlFile current_page;
			current_page.file_name = game_folder + "\\page" + Int2Str(page_num) + ".htm";
			string page_url        = base_url + "/forums/forum/" + games_to_explore[i];
			
			if (page_num !=0)
				page_url += "/?page=" + Int2Str(page_num+1);
			
			HtmlFile_Get(current_page, page_url, SKIP_IF_EXISTS);
			
			size_t tmp_offset   = 0;
			current_page.buffer = GetTextBetween(current_page.buffer, "<div class='ipsBox'", "<div class='ipsResponsive_showPhone", tmp_offset);
			
			HtmlFile_Search(current_page, "<div class='ipsDataItem_main'>", "<ul class='ipsDataItem_stats'>");
			
			for (int j=0; j<current_page.found_strings.size(); j++) {
				HtmlFile item_page;
				item_page.file_name = game_folder + "\\item" + Int2Str(item_num) + ".htm";
				
				if (current_page.found_strings[j].find("title='Pinned'") != string::npos)
					continue;
				
				size_t tmp_offset2 = 0;
				string item_url = GetTextBetween(current_page.found_strings[j], "<a href='", "'", tmp_offset2);
				
				HtmlFile_Get(item_page, item_url, SKIP_IF_EXISTS);
				
				/*FILE *file = fopen(item_page.file_name.c_str(),"rb");
				fseek(file, 0, SEEK_END);
				int file_size = ftell(file);
				fclose(file);
				
				if (file_size == 0) {
					FILE *fd = fopen("output.txt","a");
					fprintf(fd,"page_num:%d\nitem_num:%d\n%s\n%s\n\n", page_num, item_num, page_url.c_str(), item_url.c_str());
					fclose(fd);
					HtmlFile_Get(item_page, item_url, REDOWNLOAD);
				}*/
				
				item_num++;
			}
			
			tmp_offset = 0;
			if (GetTextBetween(current_page.buffer, "' rel=\"next\" data-page='", "<li class='ipsPagination_next'><a href='", tmp_offset, REVERSE).empty())
				next_page = false;
			else					
				page_num++;
		}
	}

	

	vector<string> item_properties;
	vector<string> item_properties_html;
		//title
		item_properties_html.push_back("<span class='ipsType_break ipsContained'>");
		item_properties_html.push_back("</span>");
		
		//filename
		item_properties_html.push_back("");
		item_properties_html.push_back("");
		
		//filesize
		item_properties_html.push_back("");
		item_properties_html.push_back("");
		
		//datetime
		item_properties_html.push_back("<time datetime='");
		item_properties_html.push_back("' title=");
		
		//downloads
		item_properties_html.push_back("");
		item_properties_html.push_back("");
		
		//description
		item_properties_html.push_back("<div data-role='commentContent' class='ipsType_normal ipsType_richText ipsContained' data-controller='core.front.core.lightboxedImages'>");
		item_properties_html.push_back("<div class='ipsItemControls'>");
			
	string item_buffer = "";
	string iteminfo_buffer = "";
	int record_id = 0;
	
	fstream output;
	output.open(base_folder+".sql", ios::out | ios::trunc);
	output << "CREATE TABLE IF NOT EXISTS `" << base_folder << "` ("
  "`ID` int(11) NOT NULL,"
  "`Title` text NOT NULL,"
  "`Permalink` text NOT NULL,"
  "`ForumID` int(11) NOT NULL,"
  "`LocalItem` int(11) NOT NULL,"
  "`Date` datetime NOT NULL,"
  "`Author` text NOT NULL,"
  "`Description` text NOT NULL"
") ENGINE=MyISAM DEFAULT CHARSET=utf8;" << endl << 
"ALTER TABLE `" << base_folder << "` ADD PRIMARY KEY (`ID`);" << endl << 
"INSERT INTO `" << base_folder << "` (`ID`, `Title`, `Permalink`, `ForumID`, `LocalItem`, `Date`, `Author`, `Description`) VALUES" << endl;

	for (int i=0; i<games_to_explore.size(); i++) {
		int item_id = 0;
		
		do {
			string item_file_name = base_folder + "\\" + games_to_explore[i] + "\\item" + Int2Str(item_id) + ".htm";
			
			if (GetFileAttributes(item_file_name.c_str()) == INVALID_FILE_ATTRIBUTES) {
				break;
			}
			
			Read(item_file_name, item_buffer);
			item_properties.clear();
			
			if (!item_buffer.empty()) {
				for (int j=0; j<item_properties_html.size()-1; j+=2) {
					if (item_properties_html[j].empty()) {
						item_properties.push_back("");
						continue;
					}
					
					size_t tmp = 0;
					string item_property = GetTextBetween(item_buffer, item_properties_html[j], item_properties_html[j+1], tmp);							
					item_properties.push_back(Trim(HandleQuotes(item_property, "'", "\\'")));
				}
				
				size_t tmp_offset = 0;
				string permalink  = GetTextBetween(item_buffer, "<meta property=\"og:url\" content=\"", "\"", tmp_offset);
				permalink = permalink.substr(40);
				permalink = permalink.substr(0, permalink.length()-1);
				
				tmp_offset = 0;
				string author_line = GetTextBetween(item_buffer, "<a href='https://forums.bohemia.net/profile/", "</span>", tmp_offset);
				tmp_offset = 0;
				string author = GetTextBetween(author_line, "class=\"ipsType_break\">", "</a>", tmp_offset);
				
				char *title = (char *)item_properties[TITLE].c_str();
				char *description = (char *)item_properties[DESCRIPTION].c_str();
				
				if (record_id != 0)
					output << "," << endl;
				
				output 
				<< "(" 
				<< record_id 
				<< ",'" 
				<< item_properties[TITLE].substr(6)
				<< "','" 
				<< permalink
				<< "'," 
				<< i
				<< "," 
				<< item_id
				<< ","
				<< "CONVERT('" << item_properties[DATETIME].substr(0, item_properties[DATETIME].length()-1) << "',datetime)"
				<< ",'" 
				<< author
				<< "','" 
				<< item_properties[DESCRIPTION].substr(0, item_properties[DESCRIPTION].length()-11)
				<< "')";
				
				record_id++;
			}
			
			item_id++;
		} while(true);
	}
	
	output << ";";
	
	output.close();
}

int main(int argc, char *argv[]) {
	//DSServers();
	//LoneBullet();
	//ModDB();
	BIForum();
	return 0;
}
