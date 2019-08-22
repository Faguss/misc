// Program for obtaining player list from the OFP dedicated server window
// by Faguss (ofp-faguss.com) 02.07.15

	// Headers
#include <fstream>		// file operations
#include <vector>		// vectors
#include <iostream>     // display text
#include <fstream>		// file operations
#include <windows.h>	// winapi
#include <tlhelp32.h>	// process/module traversing

	// Namespaces
using namespace std;





// **************** FUNCTIONS **************************************************

//http://my.fit.edu/~vkepuska/ece5527/sctk-2.3-rc1/src/rfilter1/include/strncmpi.c
int strncmpi(char *ps1, char *ps2, int n)
{
	char *px1=ps1, *px2=ps2;
	int indicator=9999, i=0;	

	while (indicator == 9999)
	{
		if (++i > n) indicator=0;
		else
		{
			if (*px1 == '\0')
			{
				if (*px2 == '\0') indicator=0; else	indicator=-1;
			}
			else
			{
				if (toupper((int)*px1) < toupper((int)*px2)) indicator=-1; 
				else
				{
					if (toupper((int)*px1) > toupper((int)*px2)) indicator=1; else px1+=1, px2+=1;
				};
			};
		};
	};

	return indicator;
};

















// **************** MAIN PROGRAM ***********************************************

int main(int argc, char *argv[])
{
	// Set working directory to game root folder -------------------
	size_t found;
	TCHAR pwd[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, pwd);
	string exepath = pwd;
	if (exepath.substr(exepath.size()-1,1) != "\\")
		exepath = exepath + "\\";
	found = exepath.find("fwatch\\data");
	if (found!=string::npos)
		exepath = exepath + "..\\..\\";	
	SetCurrentDirectory( exepath.c_str() );
	cout << exepath << endl << endl << endl;
	system("pause");
	return 1;
	
	
	// Process arguments -------------------------------------------
	bool setGame=false,  CWA=false;
	for (int i=1; i<argc; i++)
	{
		// if Fwatch -nolaunch then instead of pid we only get version information
		if (strncmp(argv[i],"-cwa=",5) == 0) 
			setGame = true,
			CWA = atoi(argv[i]+5);
	};

	// Main loop ---------------------------------------------------
	HWND hwnd = NULL;
	bool found = false,  gotPath = false;
	DWORD pid = 0;
	
	while (true)
	{
		Sleep(2000);

		// Search for the dedicated server window
		if (!found) 
		{
			// Iterate windows
			if (hwnd = GetTopWindow(hwnd)) 
			{
				char windowName[64] = "";
				do
				{
					GetWindowText(hwnd, windowName, 64);
					bool isOFP = !strncmpi(windowName,"operation flashpoint console",28),
						 isCWA = !strncmpi(windowName,"operation flashpoint console",24);
						 
					// if not specified game then any game otherwise game must match passed argument
					if (!setGame && (isOFP || isCWA)  ||  setGame && !CWA && isOFP  ||  setGame && CWA && isCWA) 
					{
						GetWindowThreadProcessId(hwnd, &pid);
						found = true; 
						break;
					};
				}
				while (hwnd = GetNextWindow(hwnd, GW_HWNDNEXT));
			};
			
			// Get game path
			
			
			if (gotPath)
				cout << filename;
		};
	};
	return 1;


    const int size = 1024 * 1024;
    char buffer[size];
    char lastMessage[256] = "";
    char *bufferPos = buffer;
    HWND findWindow = GetWindow(hwnd,GW_CHILD);
    vector<string> Nick, ID;
    vector<string>::iterator r, s;
    char Admin[30]="", AdminID[30]="";
    char temp[128]="";
    bool update = true;
    int updateNum = 0;
    ofstream listFile;
    
        
    while(found)
    {
        SendMessage(findWindow,WM_GETTEXT,sizeof(buffer),(LPARAM)buffer);
        if (strcmp(lastMessage,"")!=0)
        {
            bufferPos = strstr(buffer,lastMessage);
            if (bufferPos == NULL)
                bufferPos=buffer;
            else
                bufferPos+=strlen(lastMessage);
        };
        
        //cout << buffer << endl << endl;
        
        /*for (int i=0; i<strlen(buffer); i++)
        {
            printf("%c - 0x%x\n",buffer[i],buffer[i]);
        };*/
                
        char *p = strtok(bufferPos,"\r\n");
        while (p != NULL)
        {
            // skip date
            for (; !isalpha(p[0]); p++);
            
            // If admin
            if (strncmp(p,"Admin ",6)==0)
            {
                // entered password
                if (!strncmp("logged in.", p+strlen(p)-10, 10))
                    strncpy(Admin, p+6, strlen(p)-6-10);
                
                // logged out    
                if (!strncmp("logged out.", p+strlen(p)-11, 11))
                    strcpy(Admin,"");
    
                cout << "Admin: " << Admin << endl;
                update = true;
            };
            
            // If player
            if (strncmp(p,"Player ",7)==0)
            {               
                // came to the server
                char *p2 = strstr(p,"connected (id=");
                if (p2 != NULL)
                {                   
                    int pos = p2 - p;
                    strncpy(temp, p+7, pos-8);
                    Nick.push_back(temp);
                    cout << temp << " ";
                    
                    strncpy(temp, p2+14, strlen(p2)-14-1 );
                    ID.push_back(temp);
                    
                    cout << temp << endl;
                    update = true;
                };
                
                // left the server
                if (!strncmp("disconnected", p+strlen(p)-12, 12))
                {                  
                    // find name in vector and erase
                    strncpy(temp, p+7, strlen(p)-7-13);
                    cout << temp << "left" << endl;
                    int i=0;
                    for (vector<string>::iterator r=Nick.begin(); r!=Nick.end(); r++, i++)
                        if (*r == temp)
                        {
                            Nick.erase(r);
                            ID.erase(ID.begin()+i);
                            break;
                        };
                        
                    update = true;
                };
            };
            
            /*
            Admin %s logged in.
            All users disconnected, waiting for users
            Mission %s read from bank
            Mission %s read from directory
            Roles assigned
            Reading mission ...
            Error loading mission
            Mission read
            Game started
            Game restarted
            Game finished
            Waiting for next game
            Server identity created
            Player %s connecting
            Player %s connected (id=%s)
            Admin login rejected for %s
              Reason: Too many attempts with bad passwords
            Admin %s logged in.
            Admin %s logged out.
            Player %s disconnected
            Player %s kicked off - too big custom file '%s' (%d B > %d B)
            Player %s kicked off - invalid custom file '%s'
            */
            
            strncpy(lastMessage, p, 255);
            p = strtok(NULL,"\r\n");
        };
        
        Sleep(2000);
        
        if (strlen(buffer) >= size-100)
        {
            strcpy(buffer,"");
            SendMessage(findWindow,WM_SETTEXT,sizeof(buffer),(LPARAM)buffer);
        };

        // Write list of players to a text file
        if (update)
        {
            cout << "update file (" << (updateNum+1) << ")" << endl;
            listFile.open ("fwatch\\tmp\\serverplrlist.sqf", ios::out | ios::trunc);
            
            // write version of this file
            listFile << "_VER=" << ++updateNum << ";";
                            
            // Find admin id
            for (r=Nick.begin(), s=ID.begin(); r!=Nick.end(); r++, s++)
                if (*r == Admin)
                    listFile << "_ADMIN=[\"" << *r << "\",\"" << *s << "\"];";
            
            // Write list of players
            listFile << "[";
            for (r=Nick.begin(), s=ID.begin(); r!=Nick.end(); r++, s++)
            {
                if (r != Nick.begin()) listFile << ",";
                listFile << "[\"" << *r << "\",\"" << *s << "\"]";
            };
            
            listFile.close();
            update = false;
        };
	   
        
            

        cout << endl << endl;
    };
    
    return 1;
};
