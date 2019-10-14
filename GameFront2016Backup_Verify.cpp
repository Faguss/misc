// Program for verifying GameFront OFP backup
// 1. Check if files listed in MainList.txt really exist in the sub-directories
// 2. Check if files listed in MainList.txt are also listed in _Info.txt



// Headers:
#include "stdafx.h"		// VS'13
#include <list>			// std::list
#include <windows.h>    // winapi
#include <filesystem>	// directory iterator

// Namespaces:
using namespace std;
using namespace std::tr2::sys;		// directory iterator

// Global variables:
ofstream logfile;





// Functions:
//-------------------------------------------------------------------------------
// Open main list, extract titles & names and put them to list
void MainListToSTDList(string filename, list<string>& data)
{
	// Open file
	fstream listfile;
	listfile.open(filename, ios::in);

	if (!listfile.is_open())
		return;

	// Copy text to a string
	string content((istreambuf_iterator<char>(listfile)), istreambuf_iterator<char>());

	// Parse
	size_t uploaded = content.find("Uploaded:");

	while (uploaded != string::npos)
	{
		// Line after "uploaded" contains title
		size_t TitleStart = content.find("\n", uploaded);
		size_t TitleEnd	  = content.find("\n", TitleStart+1);

		// Extract title
		string Title	  = content.substr(TitleStart+1, TitleEnd-TitleStart-1);

		// Two lines after title there is a filename
		size_t FileStart  = content.find_first_not_of(" \t\f\v\n\r", TitleEnd);
		size_t FileEnd	  = content.find(" |", FileStart);

		// Extract filename
		string FileName	  = content.substr(FileStart, FileEnd-FileStart);

		// Copy extracted data to the list
		data.push_back(Title + "\a" + FileName);

		// Find next occurence
		uploaded = content.find("Uploaded:", uploaded+1);
	};

	listfile.close();
};
//-------------------------------------------------------------------------------





//-------------------------------------------------------------------------------
// Open info list, extract all titles and put them to a list
void InfoTextToSTDList(string filename, list<string>& titles)
{
	//logfile << "\tfile:" << filename << endl;

	// Open file
	fstream info;
	info.open(filename, ios::in);

	if (!info.is_open())
		return;

	// Variables for parsing
	string line;					// holds current line
	bool nextIsTitle	 = true;	// copy line to list
	bool nextIsSeparator = false;	// switch nextIsTitle

	// For each line
	while (getline(info, line))
	{
		//logfile << line << endl;

		// If it's a title
		if (nextIsTitle  &&  !isspace(line[0]))
		{
			//logfile << line << endl;

			// Copy to list
			titles.push_back(line);

			// Condition to quit particular file to prevent crashing
			if (filename.compare(".\\Modifications\\Add Ons\\Weapons\\_Info.txt") == 0  &&  line == "[PSOL] DesertEagle.zip")
				break;

			nextIsTitle = false;
		};

		// "Popularity" precedes separator
		if (line.compare(0, 11, "Popularity:") == 0)
			nextIsSeparator = true;

		// If it's a item separator
		if (nextIsSeparator  &&  line.compare(0, 3, "===") == 0)
			nextIsSeparator = false,
			nextIsTitle		= true;
	};

	info.close();
};
//-------------------------------------------------------------------------------





//-------------------------------------------------------------------------------
// Log list contents
void STDListToLog(list<string>& var)
{
	for (list<string>::iterator q = var.begin(); q != var.end(); q++)
	{
		// Replace \a with dash
		string currentRecord = *q;
		size_t separator	 = currentRecord.find("\a");

		if (separator != string::npos)
			currentRecord.replace(separator, 1, " - ");

		logfile << "\t" << currentRecord << endl;
	};
};
//-------------------------------------------------------------------------------






//-------------------------------------------------------------------------------
// Our program
int _tmain(int argc, _TCHAR* argv[])
{
	// Start logging
	logfile.open("logfile.txt", ios::out | ios::trunc);


	// Create lists
	list<string> data;
	list<string> data2;
	list<string> InfoTitle;

	// Fill list
	MainListToSTDList("MainList.txt", data);


	// Find out if this is the "Operation Flashpoint" folder
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string exepath = string(buffer);

	// If it is then we need to remove files from the other games
	if (exepath.find("\\Operation Flashpoint\\") != string::npos)
	{
		// list of other directories
		list<string> dirs
		{ 
			"Operation Flashpoint 2 Dragon Rising",
			"Operation Flashpoint Dragon Rising Red River",
			"Operation Flashpoint Elite",
			"Operation Flashpoint Red River",
			"Operation Flashpoint Resistance" 
		};

		// temporary list
		list<string> tmp_data;

		// iterate through the dir list
		for (list<string>::iterator q = dirs.begin(); q != dirs.end(); q++)
		{
			tmp_data.clear();

			// Get main lists from the other folders to a temporary list
			string filename = "..\\" + *q + "\\MainList.txt";
			MainListToSTDList(filename, tmp_data);

			// for each entry in a temporary list
			for (list<string>::iterator r = tmp_data.begin(); r != tmp_data.end(); r++)
			{
				//logfile << "\t" << *r << endl;

				// Get filename and title from the current record
				string currentRecord = *r;
				size_t separator	 = currentRecord.find_first_of("\a");
				string tmp_title	 = currentRecord.substr(0, separator);
				string tmp_filename  = currentRecord.substr(separator + 1);
				
				// search for it in the main list
				for (list<string>::iterator p = data.begin(); p != data.end(); p++)
				{
					//logfile << "\t\t" << *p << endl;

					// Get filename and title from the current record
					string currentRecord = *p;
					size_t separator	 = currentRecord.find_first_of("\a");
					string data_title	 = currentRecord.substr(0, separator);
					string data_filename = currentRecord.substr(separator + 1);

					// if both name and title match then remove it from the main list
					if (data_title == tmp_title  &&  tmp_filename == data_filename)
					{
						//logfile << "REMOVE: " << data_title << " - " << tmp_title << "\t\t\t" << tmp_filename << " - " << data_filename << endl;

						data.erase(p);
						break;
					}
				}
			}
		}
	}
	
	// Make a copy for verifying infolists
	data2 = data;



	
	// Browse sub-directories
	for (recursive_directory_iterator i("."), end;  i!=end;  ++i)
	{
		string filename  = i->path().filename();
		string extension = filename.substr(filename.length()-3, 3);
		
		// If current file is an infolist
		if (filename.compare("_Info.txt") == 0)
			InfoTextToSTDList(i->path(), InfoTitle);

		// If current file is not a directory and not a text
		if (!is_directory(i->path())  &&  extension!="txt")
		{
			// then search for it in a list
			for (list<string>::iterator q = data.begin(); q != data.end(); )
			{
				// Get filename from the current list record
				string currentRecord = *q;
				size_t separator	 = currentRecord.find_first_of("\a");
				string dataFileName  = currentRecord.substr(separator + 1);

				// if match then remove from the list
				if (dataFileName == filename)
				{
					data.erase(q);
					break;
				}
				else
					q++;
			};
		};
	};

	
	// Save remaining list contents to the log file
	logfile << "FILES THAT WEREN'T FOUND:\n\n";
	STDListToLog(data);
			




		
	// Finish doing work with info lists
	// Subtract InfoTitle from data2

	// For each entry in data2
	for (list<string>::iterator q = data2.begin(); q != data2.end(); )
	{
		//logfile << "item to find: " << *q << endl;
		bool found = false;

		// Get filename from the current list record
		string currentRecord = *q;
		size_t separator	 = currentRecord.find_first_of("\a");
		string dataTitle	 = currentRecord.substr(0, separator);

		// search for it in the info list
		for (list<string>::iterator r = InfoTitle.begin(); r != InfoTitle.end();)
		{
			//logfile << "\titerating: " << *r << endl;

			string currentTitle = *r;

			// if match then remove
			if (currentTitle.compare(0, currentTitle.length(), dataTitle) == 0)
			{
				found = true;
				q = data2.erase(q);
				InfoTitle.erase(r);
				break;
			}
			else
				r++;
		};

		if (!found)
			q++;
	};

	// Output result
	logfile << "\n\n\n\nINFO THAT WASN'T FOUND:\n\n" << "data2:\n\n";
	STDListToLog(data2);

	logfile << "\n\nInfoTitle:\n\n";
	STDListToLog(InfoTitle);
	

	logfile.close();
	return 0;
}
//-------------------------------------------------------------------------------