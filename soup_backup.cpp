/*
Program I used to backup soup.io posts from links stored in a text file
- metadata is saved in table.sql
- don't change order of lines in soup.txt and table.sql
- will retry failed downloads when launched the next time


CREATE TABLE `soup_posts` (
  `id` int(11) NOT NULL,
  `permalink` text COLLATE utf8_unicode_ci NOT NULL,
  `content_type` int(11) NOT NULL,
  `date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `html_body` text COLLATE utf8_unicode_ci NOT NULL,
  `image_description` text COLLATE utf8_unicode_ci NOT NULL,
  `url` text COLLATE utf8_unicode_ci NOT NULL,
  `image_preview` text COLLATE utf8_unicode_ci NOT NULL,
  `image_source` text COLLATE utf8_unicode_ci NOT NULL,
  `tags` text COLLATE utf8_unicode_ci NOT NULL,
  `folder` text COLLATE utf8_unicode_ci NOT NULL,
  `comment` text COLLATE utf8_unicode_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

INSERT INTO soup_posts (`permalink`, `content_type`, `date`, `html_body`, `image_description`, `url`, `image_preview`, `image_source`, `tags`, `folder`, `comment`) VALUES
*/

#include <sstream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

string DOWNLOADED_FILENAME;
string DOWNLOADED_URL;
string ERROR_MESSAGE;

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

	string ret = "   - " + (string)(char*)errorText;

	if (errorText != NULL)
		LocalFree(errorText);

	return Trim(ret);
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

void Tokenize(string text, string delimiter, vector<string> &container, bool custom_delimiters=false)
{
	bool first_item   = false;
	bool inQuote      = false;
	char custom_delim = ' ';
	bool use_unQuote  = true;
	
	// Split line into parts
	for (int pos=0, begin=-1;  pos<=text.length();  pos++) {
		bool isToken = pos == text.length();
		
		for (int i=0;  !isToken && i<delimiter.length();  i++)
			if (text.substr(pos,1) == delimiter.substr(i,1))
				isToken = true;
				
		if (text.substr(pos,1) == "\"")
			inQuote = !inQuote;
			
		// Mark beginning of the word
		if (!isToken  &&  begin<0) {
			begin = pos;
			
			if (custom_delimiters) {
				if (text.substr(begin,2) == ">>") {
					begin      += 3;
					size_t end  = text.find(text[pos+2], pos+3);
					isToken     = true;
					pos         = end==string::npos ? text.length() : end;
					use_unQuote = false;
				}
			}
		}

		// Mark end of the word
		if (isToken  &&  begin>=0  &&  !inQuote) {
			string part = text.substr(begin, pos-begin);
			
			if (use_unQuote)
				part = UnQuote(part);
			else
				use_unQuote = true;
				
			container.push_back(part);
			begin = -1;
		}
	}
}

	// Read wget log to get information about download
int ParseWgetLog(string &error)
{
	fstream DownloadLog;
    DownloadLog.open("downloadLog.txt", ios::in);

	if (DownloadLog.is_open()) {
		string text        = "";
		string filesize    = "";
		bool foundFileName = false;
		string Progress[]  = {"", "", "", ""};

		while(getline(DownloadLog, text)) {
			text = Trim(text);

			if (text.empty())
				continue;

			// Get file size
			if (filesize==""  &&  text.substr(0,8) == "Length: ") {
				size_t open  = text.find('(');
				size_t close = text.find(')');

				if (open!=string::npos  &&  close!=string::npos)
					filesize = text.substr( open+1, close-open-1);
			}

			// Get progress bar
			if (text.find("0K ") != string::npos  &  text.find("% ") != string::npos) {
				vector<string> Tokens;
				Tokenize(text, " .=", Tokens);
				
				for (int i=0; i<Tokens.size(); i++)
					Progress[i] = Tokens[i];
			}

			// Get file name
			const int items = 4;
			vector<string> SearchFor[items];

			SearchFor[0].push_back("Saving to: '");
			SearchFor[0].push_back("'");

			SearchFor[1].push_back(") - '");
			SearchFor[1].push_back("' saved [");

			SearchFor[2].push_back("File '");
			SearchFor[2].push_back("' already there; not retrieving");

			SearchFor[3].push_back("Server file no newer than local file '");
			SearchFor[3].push_back("' -- not retrieving");

			for (int i=0; i<items; i++) {
				size_t search1 = text.find(SearchFor[i][0]);
				size_t search2 = text.find(SearchFor[i][1]);

				if (search1!=string::npos  &&  search2!=string::npos) {
					DOWNLOADED_FILENAME = text.substr( search1 + SearchFor[i][0].length(),  search2 - search1 - SearchFor[i][0].length());
					foundFileName       = true;
					break;
				}
			}

			// Get error message
			size_t search1 = text.find("failed");
			size_t search2 = text.find("ERROR");

			if (search1 != string::npos)
				error = text;

			if (search2 != string::npos)
				error = text.substr(search2);
				
			// Get url
			size_t separator = text.find("--  ");
			size_t protocol  = text.find("://");
			if (separator != string::npos && protocol != string::npos) {
				DOWNLOADED_URL = text.substr(separator+4);
			}
		}

		DownloadLog.close();

		/*string tosave = "Connecting...";

		if (Progress[0] != "")
			tosave = "Downloading...\\n" + 
					 DOWNLOADED_FILENAME + "\\n\\n" +
					 Progress[0] + " / " + filesize + " - " + Progress[1] + "\\n" + 
					 Progress[2] + "\\n" + 
					 Progress[3] + " left";

		WriteProgressFile(INSTALL_PROGRESS, tosave);*/
	}
	else
		return 1;
	
	return 0;
}

int Download(string url, string filename, bool overwrite=true) 
{
	if (!filename.empty() && overwrite)
		DeleteFile(filename.c_str());
	
	string arguments = " --tries=1 --output-file=downloadLog.txt --no-check-certificate ";
	
	if (!filename.empty())
		arguments += "--output-document=" + filename + " ";
		
	arguments += url;
	
	cout << "Downloading: " << url << endl;
	
	// Execute program
	PROCESS_INFORMATION pi;
    STARTUPINFO si; 
	ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &pi, sizeof(pi) );
	si.cb 		   = sizeof(si);
	si.dwFlags 	   = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	DWORD exit_code;
	
	if (CreateProcess("wget.exe", &arguments[0], NULL, NULL, false, 0, NULL, NULL, &si, &pi)) {
		Sleep(10);
	
		string message = "";
		
		do {
			ParseWgetLog(message);
			GetExitCodeProcess(pi.hProcess, &exit_code);
			Sleep(100);
		}
		while (exit_code == STILL_ACTIVE);
			
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		ParseWgetLog(message);
		
		if (exit_code != 0) {
			cout << exit_code << " - " << message << endl;
			ERROR_MESSAGE = message;
		}
		
	} else {
		int errorCode = GetLastError();
		ERROR_MESSAGE = "Failed to run wget.exe - " + FormatError(errorCode);
		cout << ERROR_MESSAGE;
	}
	
	return exit_code;
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
	int result = Download(url, filename, overwrite);
	
	if (result != 0)
		return result;
		
	return Read(filename, buffer);
}

string GetTextBetween(string &buffer, string start, string end, bool reverse=false)
{
	string out    = "";
	size_t offset = 0;
	size_t pos0   = buffer.find(start, offset);
	
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

string GetTextBetweenOffset(string &buffer, string start, string end, size_t &offset, bool reverse=false)
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

int DownloadAndMove(string url, string path) 
{
	if (path.empty())
		path = ".";
	
	int result = Download(url, "", false);
	
	if (result == 0) {
		string source            = DOWNLOADED_FILENAME;
		string destination       = path + "\\" + DOWNLOADED_FILENAME;
		wstring source_wide      = string2wide(source);
		wstring destination_wide = string2wide(destination);
		
		cout << "Moving " << source << " to " << destination << endl;
		
		result        = MoveFileExW(source_wide.c_str(), destination_wide.c_str(), 0);
		int errorCode = GetLastError();

	    if (!result && errorCode != 183) {
			ERROR_MESSAGE = "Move " + source + " " + destination + " " + FormatError(errorCode);
			cout << "  FAILED " << errorCode << " " << ERROR_MESSAGE;
			result = errorCode;
	    } else
	    	result = 0;
	}
	
	return result;
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




int main(int argc, char *argv[]) 
{
	char default_file[] = "soup.txt";
	char *filename = default_file;
	
	if (argc > 1)
		filename = argv[1];
	
    
    string save_file_new = "table_in_progress.sql";
    string save_file_old = "table.sql";
	
		
	ofstream file_output;
	file_output.open(save_file_new.c_str(), ios::out | ios::trunc);
	file_output.close();

    fstream file_input;
    file_input.open(filename, ios::in);
    
    ifstream file_records;
    file_records.open(save_file_old.c_str(), ios::in);
    
	if (file_input.is_open()) {
        int line_number          = 0;
        bool category_separator  = false;
        string category_name     = "";
        string sub_category_name = "";
        string path              = "";
    	string text_line         = "";
    	string records_line      = "";

		while(getline(file_input, text_line)) {
			records_line = "";
			
			if (file_records.is_open())
				getline(file_records, records_line);
			
			line_number++;
			
			
			size_t protocol    = text_line.find("http");
			size_t soup_link   = text_line.find("soup.io/post");
			size_t direct_link = text_line.find("soupcdn.com/");
			
			if (protocol!=string::npos && protocol>0 && !isspace(text_line.at(protocol-1)))
				protocol = string::npos;

			bool is_valid_link = (protocol!=string::npos  &&  soup_link!=string::npos)  ||  direct_link!=string::npos;
			
			
			if (protocol == string::npos) {
				if (text_line[0] == '-' || text_line[0] == '=') {
					category_separator = true;
					path = "";
				}
				
				if (text_line[0] == '\t' && !Trim(text_line).empty()) {
					if (category_separator) {
						category_separator  = false;
						category_name       = Trim(text_line);
						path                = category_name;
						wstring source_wide = string2wide(path);
						CreateDirectoryW(source_wide.c_str(), NULL);
					} else {
						sub_category_name   = Trim(text_line);
						path                = (category_name.empty() ? "." : category_name) + "\\" + sub_category_name;
						wstring source_wide = string2wide(path);
						CreateDirectoryW(source_wide.c_str(), NULL);
					}
				}
			}


			if ((records_line.empty() || records_line.substr(0,7)=="--ERROR")  &&  is_valid_link) {
				cout << "Line " << line_number << endl;
				
				records_line = "--ERROR";
				string url = "";
				size_t end = 0;
				
				for (int i=protocol; i<text_line.length(); i++) {
					end = i+1;
					if (isspace(text_line.at(i)))
						break;
				}
				
				if (direct_link != string::npos) {
					size_t asset2 = text_line.find("/asset/");
					
					if (asset2 != string::npos)
						url = "https://whatthepost.soup.io" + text_line.substr(asset2, end-asset2);
				} else
					url = text_line.substr(protocol, end-protocol);
				
				string current_page = "";
				int result = Get(url, "current_page.htm", current_page);
				//int result = Read("current_page.htm", current_page);
				
				string post_url = DOWNLOADED_URL;
				
				if (result == 0) {					
					enum POST_TYPES {
						POST_UNKNOWN,
						POST_TEXT,
						POST_IMAGE,
						POST_VIDEO
					};
					
					int post_type      = POST_UNKNOWN;
					bool content_saved = false;
					
					if (current_page.find("Currently, soup.io is under heavy usage") != string::npos)
						current_page = "";
					
					string date            = GetTextBetween(current_page, "<span class=\"time\"><abbr title=\"", "\"");			
					current_page           = GetTextBetween(current_page, "<!--soup _post_full.html -->", "<!--soup _post_actions.html -->");
					string description     = GetTextBetween(current_page, "<div class=\"description\">", "</div>");
					string image_container = GetTextBetween(current_page, "<div class=\"imagecontainer\"", "</div>");
					string video           = GetTextBetween(current_page, "<video", "</video>");
					
					string body            = "";
					string image_url       = "";
					string image_url_small = "";
					
					if (!image_container.empty()) {
						post_type       = POST_IMAGE;
						image_url_small = GetTextBetween(image_container, "src=\"", "\"");
						
						if (image_container.find("lightbox") != string::npos)
							image_url = GetTextBetween(image_container, "href=\"", "\"");
						
						if (image_url.empty())
							image_url = image_url_small;
						
						
						content_saved = DownloadAndMove(image_url, path)==0; 
					} else 
						if (!video.empty()) {
							post_type     = POST_VIDEO;
							image_url     = GetTextBetween(image_container, "src=\"", "\"");
							content_saved = DownloadAndMove(image_url, path)==0; 
						} else {
							size_t body_pos = current_page.find("<span class=\"body\">");
							
							if (body_pos == string::npos)
								body_pos = current_page.find("<div class=\"body\">");
						
							if (body_pos != string::npos) {
								body          = current_page.substr(body_pos);
								post_type     = POST_TEXT;
								content_saved = true;
							}
						}
					
					string source_container = GetTextBetween(current_page, "<div class=\"caption\">", "</div>");
					string source_url       = "";
					
					if (!source_container.empty())
						source_url = GetTextBetween(source_container, "href=\"", "\"");
						
						
					string tags_container = GetTextBetween(current_page, "<div class=\"tags\">", "</div>");
					string tags           = "";
					
					if (!tags_container.empty()) {
						size_t offset     = 0;
						string single_tag = "";
						
						do {
							single_tag = GetTextBetweenOffset(tags_container, "</a>", ">", offset, true);
							tags      += single_tag + " ";
						} while (!single_tag.empty());
					}
					
					if (!date.empty())
						date = "STR_TO_DATE('" + date + "','%b %d %Y %T UTC')";
					else
						date = "''";

					if (content_saved) {						
						records_line = 
						"('" + Trim(post_url) + 
						"'," + Int2Str(post_type) + 
						"," + date + 
						",'" + HandleQuotes(body, "'", "\\'") + 
						"','" + HandleQuotes(description, "'", "\\'") + 
						"','" + image_url + 
						"','" + image_url_small + 
						"','" + source_url + 
						"','" + Trim(tags) + 
						"','" + ReplaceAll(path, "\\", "\\\\") + 
						"','" + HandleQuotes(Trim(text_line.substr(end)), "'", "\\'") + 
						"'),";
					} else {
						cout << "Content not saved: " << ERROR_MESSAGE << endl;						
						records_line = post_type == POST_UNKNOWN ? "--ERROR - UNKNOWN POST" : ("--ERROR - " + ERROR_MESSAGE);
					}
				} else
					records_line = "--ERROR - " + ERROR_MESSAGE;
			} else
				if (!is_valid_link)
					records_line = "-- " + text_line;
			
			file_output.open(save_file_new.c_str(), ios::out | ios::app);
			file_output << records_line << endl;
			file_output.close();
		}
				
		file_input.close();
	}
	
	file_records.close();
	MoveFileEx(save_file_new.c_str(), save_file_old.c_str(), MOVEFILE_REPLACE_EXISTING);
	return 0;
}
