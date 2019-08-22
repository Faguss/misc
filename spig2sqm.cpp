// Source code for spig2sqm v0.7 by Faguss (ofp-faguss.com)


    // Headers

#include <fstream>
#include <vector>
#include <unistd.h>     // for access command
#include <windows.h>	// winapi



    // Name spaces

using namespace std;



    // Declare functions

int    String2Int (string in);
string Int2String (int in);
string lowcase (string out);
string formatProperty (string in, int id);
string quote2bracket (string in);
void   writeClasses (string itemF1s, string itemF2s);
void   Message(string in);



    // Declare global variables

vector<string> T[4096];
fstream file1;
fstream file2;
fstream file3;
string fullpath  = "";
string directory = "";
string height 	 = "";
bool silent 	 = false;                        // Silent Mode (no messages) disabled by default
int pos 		 = 0;







int main(int argc, char *argv[])
{
   
	// Check if any argument is -silent to enabled Silent Mode
	for(int i=1; i<argc; i++)
	{  
		string currARG = string(argv[i]);

		if (currARG == "-silent") 
			silent = true;
	};
	
	// if no argument given
	if (argc < 2) 
	{
		Message("No save file to read"); 
		return -1;
	}
        
     
        
        
	// open save file for reading
	// end if failed
	file1.open(argv[1], ios::in);

	if (!file1) 
	{
		FILE *fd = fopen("log.txt","a");
            
		for (int i=0; i<argc; i++)
			fprintf(fd, "%s ", argv[i]);
            
		fprintf(fd, "\n%d - %s\n\n", errno, strerror(errno));
		fclose(fd);
            
		Message("Failed to read save file"); 
		return -2;
	};
        
	// check if mission.sqm exists
	int missionsqm = access( "mission.sqm", 0 );
        

	// if not then maybe it's drag & drop
	// and work directory is different
	if (missionsqm == -1)
	{
		// get program directory
		size_t find;
		
		directory = argv[0];
		find 	  = directory.find_last_of("/\\");
		directory = directory.substr(0,find) + "\\";      
		fullpath  = directory + "mission.sqm";
		
		if (access(fullpath.c_str(),0) == -1) 
		{
			FILE *fd = fopen("log.txt","a");
            
			for (int i=0; i<argc; i++)
				fprintf(fd, "%s ", argv[i]);
				
			TCHAR pwd[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, pwd);
			fprintf(fd, "\n%s \n\n", pwd);
			fclose(fd);
			
			Message("Could not find mission.sqm"); 
			return -3;
		};
	};
        
        
	// make backup of the original mission.sqm
	fullpath 		 = directory + "mission.sqm";
	string fullpath2 = directory + "mission.bak";
	int backupNumber = 1;
	
	while ( rename(fullpath.c_str() , fullpath2.c_str()) != 0 )
	{
		fullpath2 = directory + "mission" + Int2String(backupNumber) + ".bak";
		backupNumber++;
	};
        
	// open it for reading
	file2.open(fullpath2.c_str(), ios::in);  
        


        
       
	// create file3 - new mission.sqm
	fullpath = directory + "mission.sqm";
	ofstream write(fullpath.c_str());
        
	// open file3 for writing
	file3.open(fullpath.c_str(), ios::out);
        
	// end if couldn't open the file
	if (!file3) 
	{
		FILE *fd = fopen("log.txt","a");
            
		for (int i=0; i<argc; i++)
			fprintf(fd, "%s ", argv[i]);
            
		fprintf(fd, "\n%d - %s\n\n", errno, strerror(errno));
		fclose(fd);
		
		Message("Failed to create file"); 
		return -4;
	};
        
        
        
        
        
       

       
        





	// First part: read save file, save variables to vector
	// item<FILE><type>
	// row is which row in save file
	// pos is position in vector
     
	string line    = "";
	string itemF1s = "";
	bool inQuote   = false;
    int row 	   = 0;
       
       
	while(getline(file1,line))
	{
		if (row < 2) 
		{
			row++; 
			continue;
		};
		
		string j 		  = "";
		bool startWriting = false;
		int dl 	 		  = line.length();
           
		for (int i=0; i<=dl; i++)
		{
			// inside quotation marks
			if (line[i] == '"') 
				inQuote = !inQuote;
				
			// start saving vars to vector
			if (line[i]=='['  &&  !startWriting) 
			{
				startWriting = true; 
				continue;
			}; 
			
			if (!startWriting) 
				continue;
            
			// when encounter end of value character			
			if (line[i]==',' && !inQuote  ||  line[i]==']' && !inQuote)
			{
				if (row==2  &&  pos==1) 
					itemF1s=j;
					
				T[pos].push_back(j);
				j = "";
				pos++;
				continue;
			};
              
			// save single character to string j
			j += line[i];
		};
           
		row++;
	};   



		
	// Second part: read sqm, find class vehicles, add vars from vector
	// bools used to find number of items in sqm
	// level - inside of how many brackets

	string itemF2s 	   = "";
	string itemF3s 	   = "";
	bool saveItems 	   = false;
	bool classVehicles = false;
	bool addedItems    = false;
	int level 		   = 0;


	while (getline(file2,line))
	{
		string j = "";
		int dl 	 = line.length();
           
		for (int i=0; i<=dl; i++)
		{
			if (line[i] == '{') 
				level++;
				
			if (line[i] == '}') 
				level--;

				
			// when got number of items
			if (i==dl-1  &&  saveItems)                                         
			{
				saveItems 	  = false;
				classVehicles = false;
				addedItems 	  = true;
				
				// function to add new objects
				writeClasses(itemF1s,itemF2s);
				
				j = "";
				break;
			};

			
			// skip tab and new line chars
			if (line[i]==0x09  ||  line[i]==0x00) 
				continue;
              
  
			// save char to one of the two vars
			if (saveItems) 
				itemF2s = itemF2s + line[i];
			else 
				j += line[i];
    
	
			// create class vehicles if it doesn't exist
			if (line[i]=='}'  &&  level==0  &&  !addedItems)
			{
				file3 << "class vehicles\n{\n";
				saveItems 	  = false;
				classVehicles = false;
				addedItems 	  = true;
				writeClasses(itemF1s,"0");     
				file3 << "\n};\n";
			};
              
			// in the correct class vehicles
			if (lowcase(j)=="class vehicles"  &&  level==1) 
				classVehicles = true;
				
			// found phrase items=	
			if (lowcase(j)=="items="  &&  classVehicles) 
				saveItems = true;
		};


		// save read chars to a new file
		file3 << j << "\n";
	};


	// close streams
	file1.close();
	file2.close(); 
	file3.close();
	return 1;
} 












// Format string characters to lowercase
string lowcase(string out)
{
	for (int i=0; i<out.length(); i++)
		out[i] = tolower(out[i]);

	return out;       
}


// Format string to integer
int String2Int (string in)
{
	return atoi(in.c_str());
};


// Format integer to string
string Int2String (int in)
{
	string tmp;
	sprintf((char*)tmp.c_str(), "%d", in);
	return tmp.c_str();
};


// Return property text value
string formatProperty(string in, int id)
{
	if (id == 15)
	{                    
		if (in == "1") return "CORPORAL";
		if (in == "2") return "SERGEANT";
		if (in == "3") return "LIEUTENANT";
		if (in == "4") return "CAPTAIN";
		if (in == "5") return "MAJOR";
		if (in == "6") return "COLONEL";
	};
	
	if (id == 14)
	{
		if (in == "1") return "PLAYER COMMANDER";
		if (in == "2") return "PLAYER COMMANDER";
		if (in == "3") return "PLAYER DRIVER";
		if (in == "4") return "PLAYER GUNNER";
		if (in == "5") return "PLAY CDG";
		if (in == "6") return "PLAY C";
		if (in == "7") return "PLAY D";
		if (in == "8") return "PLAY G";
		if (in == "9") return "PLAY CD";
		if (in == "10") return "PLAY CG";
		if (in == "11") return "PLAY DG";
	};
	
	if (id == 13)
	{
		if (in == "1") return "NONE";
		if (in == "2") return "CARGO";
		if (in == "3") return "FLY";
	};
	
	if (id == 12)
	{
		if (in == "1") return "ACTUAL";
		if (in == "2") return "5 MIN";
		if (in == "3") return "10 MIN";
		if (in == "4") return "15 MIN";
		if (in == "5") return "30 MIN";
		if (in == "6") return "60 MIN";
		if (in == "7") return "120 MIN";
	};
	
	if (id == 11)
	{
		if (in == "1") return "LOCKED";
		if (in == "2") return "UNLOCKED";
	};
       
	// presence condition
	if (id == 2)
	{
		return quote2bracket(in);
	};
       
	// init: format + add height
	if (id == 8)
	{
		// if empty then just height
		if (in == "\"\"")  
		{
			return "\"this setpos [getpos this select 0,getpos this select 1,"+height+"]\"";
		} 
		else
		{
			in = quote2bracket(in);
			replace( in.begin(), in.end(), '"', ' ' ); 
			return "\"" + in + "; this setpos [getpos this select 0,getpos this select 1," + height + "]\"";
		};
	};
       
	return "";
};




// Replace all inside quotation marks with brackets (for sripting commands)
string quote2bracket(string in)
{
	bool open = true;
	
	for (int i=1; i<(in.length()-1); i++)
		if (in[i] == '"')
		{
			if (open) 
				in[i] = '{';
			else 
				in[i] = '}';
				
			open = !open;
		};
	
	return in;       
};




// Write new classes into sqm file
void writeClasses(string itemF1s, string itemF2s)
{
	int itemF3i  = String2Int(itemF1s) + String2Int(itemF2s);
	int itemCURR = String2Int(itemF2s) - 1;
 
	// write new number items
	file3 << "items=" << itemF3i << ";\n";
                 
	// Loop for each position in vector
	int var = 20;
	
	for (int i=2; i<pos; i++)
	{
		// First element, create a new class
		if (var == 20) 
			file3 << "class Item" << ++itemCURR << endl << "{" << endl;
                     
		// Get string from vector
		for (vector<string>::iterator q=T[i].begin(); q!=T[i].end(); q++)
		{
			// Skip if default value
			if (var==15  &&  *q=="0") break;
			if (var==14  &&  *q=="0") break;
			if (var==13  &&  *q=="0") break;
			if (var==12  &&  *q=="0") break;
			if (var==11  &&  *q=="0") break;
			if (var==10  &&  *q=="\"\"") break;
			if (var==8  &&  *q=="\"\""  &&  height=="0") break;
			if (var==7  &&  *q=="100") break;
			if (var==6  &&  *q=="100") break;
			if (var==5  &&  *q=="100") break;
			if (var==4  &&  *q=="100") break;
			if (var==3  &&  *q=="0") break;
			if (var==2  &&  *q=="\"true\"") break;
			if (var==1  &&  *q=="0") break;
                        
			// Write value
			switch (var)
			{
				case 20 : file3 << "position[]={" << *q << ",0,"; break;
				case 19 : file3 << *q << "};" << endl; break;
				case 18 : height=*q; break;
				case 17 : file3 << "side=\"EMPTY\";" << endl; break;
				case 16 : file3 << "vehicle=" << *q << ";" << endl; break;
				case 15 : file3 << "rank=\"" << formatProperty(*q,var) << "\";" << endl; break;
				case 14 : file3 << "control=\"" << formatProperty(*q,var) << "\";" << endl; break;
				case 13 : file3 << "special=\"" << formatProperty(*q,var) << "\";" << endl; break;
				case 12 : file3 << "age=\"" << formatProperty(*q,var) << "\";" << endl; break;
				case 11 : file3 << "lock=\"" << formatProperty(*q,var) << "\";" << endl; break;
                           
				case 10 : file3 << "text=" << *q << ";" << endl; break;
				case 9 : file3 << "skill=" << *q << ";" << endl; break;
				case 8 : file3 << "init=" << formatProperty(*q,var) << ";" << endl; break;
				case 7 : file3 << "health=" << *q << ";" << endl; break;
				case 6 : file3 << "fuel=" << *q << ";" << endl; break;
				case 5 : file3 << "ammo=" << *q << ";" << endl; break;
				case 4 : file3 << "presence=" << *q << ";" << endl; break;
				case 3 : file3 << "azimut=" << *q << ";" << endl; break;
				case 2 : file3 << "presenceCondition=" << formatProperty(*q,var) << ";" << endl; break;
				case 1 : file3 << "placement=" << *q << ";" << endl; break;
			};
		};
		
		// Next var, if this was last then write id and close class
		var--;
		
		if (var == 0) 
		{
			file3 << "id=" << (itemCURR+1) << ";\n" << "};\n"; 
			var = 20;
		};
	}; 
};


// DOS message
void Message(string in)
{
	if (silent) 
		return;

	system("echo spig2sqm v0.5 by Faguss (ofp-faguss.com)");
	in = "echo " + in;
	system(in.c_str());
	system("echo.");
	system("pause");     
};


