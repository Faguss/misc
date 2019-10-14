// Program for verifying GameFront OFP backup
// 1. Check if files listed in MainList.txt really exist in the sub-directories
// 2. Check if files listed in MainList.txt are also listed in _Info.txt



// Headers:
#include "stdafx.h"		// VS
#include <vector>       // vectors
#include <windows.h>    // winapi
#include <filesystem>	// directory iterator

// Namespaces:
using namespace std;
using namespace std::tr2::sys;

// Global variables:
ofstream logfile;
unsigned int title_size = 0;
unsigned int name_size = 0;
unsigned int game_size = 0;
unsigned int path_size = 0;
unsigned int dupa_size = 0;
unsigned int down_size = 0;
unsigned int desc_size = 0;





// Functions:
void replaceAll(string& source, const string& from, const string& to)
{
	string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	string::size_type lastPos = 0;
	string::size_type findPos;

	while (string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
};
//-------------------------------------------------------------------------------
// Open main list, extract titles & names and put them to vectors
void ListToVector(string filename, vector<string>& titles, vector<string>& names)
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
		size_t TitleEnd = content.find("\n", TitleStart + 1);

		// Extract title
		string Title = content.substr(TitleStart + 1, TitleEnd - TitleStart - 1);

		// Two lines after title there is a filename
		size_t FileStart = content.find_first_not_of(" \t\f\v\n\r", TitleEnd);
		size_t FileEnd = content.find(" |", FileStart);

		// Extract filename
		string FileName = content.substr(FileStart, FileEnd - FileStart);

		// Copy extracted data to vectors
		titles.push_back(Title);
		names.push_back(FileName);

		// Find next occurence
		uploaded = content.find("Uploaded:", uploaded + 1);
	};

	listfile.close();
};
//-------------------------------------------------------------------------------





//-------------------------------------------------------------------------------
// Open info list, extract title and put it to a vector
void InfoToVector(string filename, string wantedtitle)
{
	//logfile << "\tfile:" << filename << endl;

	// Open file
	fstream info;
	info.open(filename, ios::in);

	if (!info.is_open())
		return;

	// Variables for parsing
	string line;					// holds current line
	string description = "";
	string filesize = "";
	string date = "";
	string downloads = "";
	string lastTitle = "";
	bool nextIsTitle = true;		// copy line to vector
	bool nextIsSeparator = false;	// switch nextIsTitle
	bool nextIsDescription = false;
	bool nextIsFilesize = false;
	bool nextIsDateAdded = false;
	bool nextisDownloads = false;
	bool found = false;

	// For each line
	while (getline(info, line))
	{
		//logfile << line << endl;

		// If it's a title
		if (nextIsTitle && !isspace(line[0]))
		{
			//logfile << line << endl;

			// Copy to vector
			if (wantedtitle == line)
				lastTitle = line,
				found = true;

			nextIsTitle = false;
		};

		if (found)
		{


			if (line.compare(0, 4, "Hide") == 0)
				nextIsDescription = false;

			if (nextIsDescription)
				description += line;

			if (!nextIsTitle  &&   line.compare(0, 12, "Description:") == 0)
				nextIsDescription = true;



			if (nextIsFilesize)
				filesize = line, nextIsFilesize = false;

			if (!nextIsTitle  &&   line.compare(0, 10, "File Size:") == 0)
				nextIsFilesize = true;



			if (nextIsDateAdded)
				date = line, nextIsDateAdded = false;

			if (!nextIsTitle  &&   line.compare(0, 11, "Date Added:") == 0)
				nextIsDateAdded = true;



			if (nextisDownloads)
				downloads = line, nextisDownloads = false;

			if (!nextIsTitle  &&   line.compare(0, 19, "Downloads All Time:") == 0)
				nextisDownloads = true;
		};



		// "Popularity" precedes separator
		if (line.compare(0, 11, "Popularity:") == 0)
			nextIsSeparator = true;

		// If it's a item separator
		if (nextIsSeparator  &&  line.compare(0, 3, "===") == 0)
		{
			// Condition to quit particular file to prevent crashing
			if (filename.compare(".\\Modifications\\Add Ons\\Weapons\\_Info.txt") == 0 && lastTitle == "[PSOL] DesertEagle.zip")
				break;

			nextIsSeparator = false;
			nextIsTitle = true;

			if (found)
				break;
		}
	};

	if (!found)
	{
		//logfile << endl  << "FAILURE" << endl;
		return;
	}

	replaceAll(description, "'", "''");
	//replaceAll(description, "\"", "\\\"");
	//description = description.replace("'", "''");

	size_t tmp1 = filesize.find_first_not_of(" ");
	filesize = filesize.substr(tmp1);

	size_t tmp2 = date.find_first_not_of(" ");
	date = date.substr(tmp2);

	string year = date.substr(6);
	string month = date.substr(0, 2); 
	string day = date.substr(3, 2);

	size_t tmp3 = downloads.find_first_not_of(" ");
	downloads = downloads.substr(tmp3);

	if (filesize.length() > dupa_size)
		dupa_size = filesize.length();

	if (downloads.length() > down_size)
		down_size = downloads.length();

	if (description.length() > desc_size)
		desc_size = description.length();

	logfile << filesize << "', '" << year << "-" << month << "-" << day << "', '" << downloads << "', '" << description << "'),";

	info.close();
};
//-------------------------------------------------------------------------------





//-------------------------------------------------------------------------------
// Log vector contents
void VectorToLog(vector<string>& var)
{
	for (vector<string>::iterator q = var.begin(); q != var.end(); q++)
	if (*q != "null")
		logfile << "\t" << *q << endl;
};
//-------------------------------------------------------------------------------






//-------------------------------------------------------------------------------
// Our program
int _tmain(int argc, _TCHAR* argv[])
{
	// Start logging
	logfile.open("..\\insert2.sql", ios::out | ios::app);


	// Create vectors
	vector<string> FileTitle;		// for verifying files
	vector<string> FileName;
	vector<string> FileTitle2;		// for verifying info lists
	vector<string> InfoTitle;

	// Fill vectors
	ListToVector("MainList.txt", FileTitle, FileName);


	// Find out if this is the "Operation Flashpoint" folder
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string exepath = string(buffer);

	// If it is then we need to remove files from the other games
	if (exepath.find("\\Operation Flashpoint\\") != string::npos)
	{
		// list of other directories
		vector<string> dirs
		{
		"Operation Flashpoint 2 Dragon Rising",
		"Operation Flashpoint Dragon Rising Red River",
		"Operation Flashpoint Elite",
		"Operation Flashpoint Red River",
		"Operation Flashpoint Resistance"
		};

		// temporary vectors
		vector<string> tmp_titles;
		vector<string> tmp_names;

		// iterate through the dir list
		for (vector<string>::iterator q = dirs.begin(); q != dirs.end(); q++)
		{
			tmp_titles.clear();
			tmp_names.clear();

			// Get lists from the other folders to a temporary vector
			string filename = "..\\" + *q + "\\MainList.txt";
			ListToVector(filename, tmp_titles, tmp_names);

			// for each entry in a temporary vector
			int i = 0;
			for (vector<string>::iterator r = tmp_titles.begin(); r != tmp_titles.end(); r++, i++)
			{
				// search for it in the main list
				int j = 0;
				for (vector<string>::iterator p = FileTitle.begin(); p != FileTitle.end(); p++, j++)

					// if both name and title match then remove it from the main list
				if (*r == *p  &&  tmp_names[i] == FileName[j])
				{
					//logfile << "REMOVE: " << *r << " - " << *p << "\t\t\t" << tmp_names[i] << " - " << FileName[j] << endl;
					FileTitle[j] = "null";
					FileName[j] = "null";
					break;
				};
			}
		}
	}

	// Make a copy for verifying infolists
	FileTitle2 = FileTitle;




	// Do fixes
	int count = 0;
	size_t lastSlash = exepath.find_last_of("\\");
	size_t secondlastSlash = exepath.find_last_of("\\", lastSlash-1);
	string game = exepath.substr(secondlastSlash+1, lastSlash - secondlastSlash-1);

	if (game == "Operation Flashpoint")
	{
		int j = 0;
		for (vector<string>::iterator q = FileName.begin(); q != FileName.end(); q++, j++)
		{
			if (*q == "Air Raid.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "Attack_at_Night-01.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "Captive_Interregation_11.Eden.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "D-Day 2.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "Dear Marina.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "TheBestOfPlans.Cain.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "therock.zip")
				FileName[j] = "therock_mp.zip";

			if (*q == "Wipeout SP.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "C--boy Pack.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "SumpfV1.0.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "Atonov124.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "ITA_SHELTER_V1.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "STTDesert_Armor(East)Pack1_00.zip")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "1-4_US Assassins.Cain.zip")
				FileName[j] = "US Assassins.zip";

			if (*q == "Co_Team Hold AirStrip By Lt.Eden.zip")
				FileName[j] = "Co_Team  Hold AirStrip By Lt.Eden.zip";

			if (*q == "Jungle_Everon_1_20.zip")
				FileName[j] = "jungle_everon_1_20.zip";

			if (*q == "legawarz_beta11_setup.exe")
				FileName[j] = "lwz_410_legawarz_beta11_setup.exe";

			if (*q == "OFP_Mission_Elimination_CoOp.zip")
				FileName[j] = "Elimination_CoOp.zip";

			if (*q == "OFP_Mission_HindAndSeek.zip")
				FileName[j] = "hindseek.zip";

			if (*q == "RussianWeaponsPack11.zip")
				FileName[j] = "russianweaponspack11.zip";

			if (*q == "opf_elite_mpg4_long.zip")
				FileName[j] = "opf__elite_mpg4_long.zip";

			if (*q == "rev.gif")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "my photos1.doc")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "«·Õ«NÀ.tif")
				FileName[j] = "null",
				FileTitle[j] = "null";
		}
	}

	if (game == "Operation Flashpoint 2 Dragon Rising")
	{
		count = 853;
		int j = 0;
		for (vector<string>::iterator q = FileName.begin(); q != FileName.end(); q++, j++)
		{
			if (*q == "ofppatch_upgrade_1_20.zip")
				FileName[j] = "ofpatch_upgrade_1_20.zip";

			if (*q == "OF_Dragon_Rising_Demo_Eng.exe")
				FileName[j] = "OperationFlashpoint_DragonRising_Demo.exe";

			if (*q == "OFPDR_UK_Launch_Trailer_Rated18_720p30.mov")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "OFPDR_Coop_Edit_280809_ESRB.mov")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "OperationFlashpoint_DR_Vehicles_Trailer.wmv")
				FileName[j] = "null",
				FileTitle[j] = "null";
		}
	}

	if (game == "Operation Flashpoint Dragon Rising Red River")
		count = 878;

	if (game == "Operation Flashpoint Elite")
	{
		count = 879;
		int j = 0;
		for (vector<string>::iterator q = FileName.begin(); q != FileName.end(); q++, j++)
		{
			if (*q == "opf_elite_mpg4_long.zip")
				FileName[j] = "opf__elite_mpg4_long.zip";
		}
	}

	if (game == "Operation Flashpoint Red River")
	{
		count = 881;
		int j = 0;
		for (vector<string>::iterator q = FileName.begin(); q != FileName.end(); q++, j++)
		{
			if (*q == "OFPRR_Biog2_Baletto_US_RatedM_Steam26_-h264 11Mbit stereo_48K.mov")
				FileName[j] = "null",
				FileTitle[j] = "null";
		}
	}

	if (game == "Operation Flashpoint Resistance")
	{
		count = 891;
		int j = 0;
		for (vector<string>::iterator q = FileName.begin(); q != FileName.end(); q++, j++)
		{
			if (*q == "sg_mod_beta.rar")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "opr_scud_addon__1.84_.rar")
				FileName[j] = "null",
				FileTitle[j] = "null";

			if (*q == "OFPR_Mission_Silent_Night_Deadly_Night_1.1.zip")
				FileName[j] = "Silent_Night_Deadly_Night_1.1.zip";

			if (*q == "opr_scud_addon__2.01_.rar")
				FileName[j] = "OPR_Scud_Addon_(2.01).rar";
		}
	}







	
	string currDir = "";

	// Browse sub-directories
	for (recursive_directory_iterator i("."), end; i != end; ++i)
	{
		currDir = i->path();

		if (!is_directory(i->path()))
		{
			size_t lastSlash = currDir.find_last_of("\\");
			currDir = currDir.substr(0, lastSlash);
			//logfile << "\nCURRDIR: " << currDir << endl << endl;
		}

		string filename = i->path().filename();
		string extension = filename.substr(filename.length() - 3, 3);

		// If current file is not a directory and not a text
		if (!is_directory(i->path()) && extension != "txt")
		{
			// then search for it in a vector
			int j = 0;
			bool found = false;
			for (vector<string>::iterator q = FileName.begin(); q != FileName.end(); q++, j++)
			{
				//logfile << filename << "\t-\t" << FileName[j] << endl;

				// if found then remove
				if (*q == filename)
				{				
					/*if (currDir == ".\\Maps Levels Missions\\Missions\\0 to 9 Titles" && FileTitle[j] == "US Assassins.zip")
					{
						logfile << "CONTINUE";
						continue;
					}*/
						
					found = true;
					string title = FileTitle[j];
					string name = FileName[j];

					replaceAll(title, "'", "''");
					replaceAll(name, "'", "''");

					/*title = title.replace("'", "''");
					name = name.replace("'", "''");*/



					string location = currDir.substr(2);
					replaceAll(location, "\\", "\\\\");

					if (title.length() > title_size)
						title_size = title.length();

					if (name.length() > name_size)
						name_size = name.length();

					if (game.length() > game_size)
						game_size = game.length();

					if (location.length() > path_size)
						path_size = location.length();

					logfile << "(" << count << ", '" << title << "', '" << name << "', '" << game << "', '" << location << "', '";

					InfoToVector(currDir + "\\_Info.txt", FileTitle[j]);
					logfile << "\n";

					FileTitle[j] = "null";
					FileName[j] = "null";
					count++;
					break;
				};
			};

			/*if (!found)
				logfile << "\nCOULDN'T FIND INFO FOR FILE: " << i->path() << endl << endl;*/
		};
	};


	// Save remaining vector contents to the log file
	/*logfile << "FILES THAT WEREN'T FOUND:\n\n";

	int j = 0;
	for (vector<string>::iterator q = FileName.begin(); q != FileName.end(); q++, j++)
	if (*q != "null")
		logfile << "\t" << *q << "  -  " << FileTitle[j] << endl;

		*/

	logfile <<
	"title_size: " << title_size << endl <<
	"name_size: " << name_size << endl <<
	"game_size: " << game_size << endl <<
	"path_size: " << path_size << endl <<
	"file_size: " << dupa_size << endl <<
	"down_size: " << down_size << endl <<
	"desc_size: " << desc_size << endl;
	


	logfile.close();
	return 0;
}
//-------------------------------------------------------------------------------