// ver1.3   28.05.13

#include <fstream>
#include <iostream>

using namespace std;

/*
exit codes:
1 - operation successful
-1 - no argument
-2 - couldn't open source file
-3 - couldn't open dest file
*/


int main(int argc, char *argv[])
{
    
        // end if no argument
        if (argc < 1) {return -1;}

        // Check if any argument is -silent to enabled Silent Mode
        bool silent=false;
        for(int i=1; i<argc; i++)
        {  
           string currARG = string(argv[i]);
           if(currARG == "-silent") {silent=true;};
        };
        
        // define file variables
        fstream file1, file2;     
        
        // open file1 for reading
        file1.open(argv[1], ios::in);

        // end if couldn't open file
        if (!file1) {return -2;};
        
        
        // Convert argument to string
        string path_name (argv[1]);
        
        // Divide file name and path to it
        string path = path_name.substr( 0, path_name.find_last_of( '\\' ) +1 );
        string name = path_name.substr( path_name.find_last_of( '\\' ) +1 );
        string new_path_name = path + "converted_" + name;
            
        // For launching from game            
        if (silent) {new_path_name = (string)argv[0] + "converted_" + name;};
       
       
       
        
        // create file2
        ofstream write(new_path_name.c_str());
        
        // open file2 for writing
        file2.open(new_path_name.c_str(), ios::out);
        
        // end if couldn't open file
        if (!file2) {return -3;};        
        
        
        // define variables
        char nextChar;
        string output;
        int line=1, Comment=0;
        bool newLine=true;


        // browse through every character in the file
        while (file1.get(nextChar))
        {
              
           // Add Comment on the beggining
           if (line == 1 && Comment == 0) {file2<<"Comment\n{\n\t";  Comment++;}
           
           // Tabulator on second line
           if (line == 2 && Comment == 1) {file2<<"\t"; Comment++;}
           
           // Close comment
           if (line == 3 && Comment == 2) {file2<<+"};\n\n"; Comment++;}
           
           
           // reset string variable
           output="";
           
           // replace 0x0A with ;\n
           if (nextChar == 0x0A) {nextChar=';'; output="\n"; line+=1;}                
           
           // insert tabulators after comma
           if (nextChar == ',' && line > 3) { output="\t\t"; }
           
           // insert additional line at the begging of fourth line
           if (line == 4 && newLine) {output+="\n"; newLine=false;}
           
                      
           // write into file
           file2 << nextChar << output;
        }




        // close streams
        file1.close();
        file2.close();
        
        return 1;
} 
