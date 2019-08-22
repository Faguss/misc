// Columnor - extract columns from fwatch saved files
// v1.0a 04.03.11 by Faguss (ofp-faguss.com)

#include <fstream>

using namespace std;


int main(int argc, char *argv[])
{
   
    
        // end if no argument
        if (argc < 2) {return 0;}
        
        // define file variables
        fstream file1, file2;
        
        // open file1 for reading
        file1.open(argv[1], ios::in);

        // end if couldn't open
        if (!file1) {return 0;};
        
        
                
        
      
        // Convert argument to string
        string path_name (argv[1]);
        
        // Divide file name and path to it
        string path = path_name.substr( 0, path_name.find_last_of( '\\' ) +1 );
        string name = path_name.substr( path_name.find_last_of( '\\' ) +1 );
        
      
        
        
        
        
        
        
        
        
        
        
        // count number of columns in file1
        string line;
        int columns=0;
        getline(file1,line);
        for(int i=0; i<=line.length(); i++)
        {
              if (line[i] == ',') columns++;
        };
        
        
        
        

        
        
        
        // read data from file1 and write to file2
        for(int i=0; i<=columns; i++)
        {
            // int i to string
            string tmp;
            sprintf((char*)tmp.c_str(), "%d", (i+1));
            
            // New file name
			string new_path_name = path + name + "_column" + tmp.c_str() + ".txt";

			// create file2
			ofstream write(new_path_name.c_str());
        
			// open file2 for writing
			file2.open(new_path_name.c_str(), ios::out);
        
			// end if couldn't open file
			if (!file2) {return 0;};
            
            
            
            // reopen file1
            file1.close();
            file1.clear();
            file1.open(argv[1], ios::in);



            // get and write columns
			bool insideBracket=false;
			int currentColumn=0;
			char nextChar;
			
			while (file1.get(nextChar))
			{
                // end of line
				if (nextChar == 0x0A)
                {
                    file2 << "\n";
                    currentColumn=0;
                    continue;
                };
				
				// brackets
				if (nextChar=='[' || nextChar==']' ) { insideBracket=!insideBracket; continue; };
				if (!insideBracket ) {  continue; };
				
				// next column
				if (nextChar==',') { currentColumn++; continue; };	
				
				// write
				if (currentColumn == i) { file2 << nextChar; };
			};
			
			

            // clear file2 stream
            file2.close();
            file2.clear();
        };
        


        // close file1 stream
        file1.close();
        
        return 1;
} 
