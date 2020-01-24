#include <cstdio>
#include <cstring>
#include <cstdlib>

//https://community.bistudio.com/wiki/FXY_File_Format
struct FxyEntry {
	unsigned short CharCode;
	unsigned short PaaFileNumber;
	unsigned short X,Y;
	unsigned short Width,Height;
} current;


int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("No filename given");
		return 0;
	}

	const int max_filename          = 512;
	char filename_new[max_filename] = "";
	
	char *path = argv[1];
	FILE *file = fopen(path, "rb");

	if (!file) {
		perror("");
		return 1;
	}
	
	char *filename = strrchr(path, '\\');
	
	if (filename == NULL)
		filename = path;
	else
		filename++;
		
	bool convert_to_txt = strstr(filename, ".fxy.txt") == NULL;
	
	if (convert_to_txt) {
		strncpy(filename_new, filename, max_filename-8);
		
		if (strlen(filename) >= max_filename-8)
			strcat(filename_new, ".fxy.txt");
		else
			strcat(filename_new, ".txt");
	} else {
		char *extension = strstr(filename, ".txt");
		int len         = extension - filename;
		
		strncpy(filename_new, filename, len>max_filename-4 ? max_filename-4 : len);
		
		if (len >= max_filename-4)
			strcat(filename_new, ".fxy");
			
	}
	
	printf("filename_new:%s\n",filename_new);
	
	FILE *file_new = fopen(filename_new, "wb");

	if (!file_new) {
		perror("");
		return 1;
	}
		
	bool first_item = true;
	char line[512]  = "";
	
	while(!feof(file) && !ferror(file)) {
		if (convert_to_txt) {
			fread(&current, sizeof(current), 1, file);
			
			if (first_item)
				first_item = false;
			else
				fprintf(file_new, "\r\n\r\n");
			
			fprintf(file_new,"CharCode=%u;\r\nPaaFileNumber=%u;\r\nX=%u;\r\nY=%u;\r\nW=%u;\r\nH=%u;", current.CharCode, current.PaaFileNumber, current.X, current.Y, current.Width, current.Height);
		} else {
			fgets(line, 512, file);
			
			char *equality  = strchr(line, '=');
			char *semicolon = strchr(line, ';');
	
			if (equality==NULL || semicolon==NULL)
				continue;
	
			line[equality - line]  = '\0';
			line[semicolon - line] = '\0';
			char *value            = equality+1;
			
			if (strcmpi(line,"CharCode") == 0) {
				if (first_item)
					first_item = false;
				else 
					fwrite(&current, sizeof(struct FxyEntry), 1, file_new);
				
				current.CharCode = atoi(value);
				continue;
			}
			
			if (strcmpi(line,"PaaFileNumber") == 0) {
				current.PaaFileNumber = atoi(value);
				continue;
			}
			
			if (strcmpi(line,"X") == 0) {
				current.X = atoi(value);
				continue;
			}
			
			if (strcmpi(line,"Y") == 0) {
				current.Y = atoi(value);
				continue;
			}
			
			if (strcmpi(line,"W") == 0) {
				current.Width = atoi(value);
				continue;
			}
			
			if (strcmpi(line,"H") == 0) {
				current.Height = atoi(value);
				continue;
			}
			
		}
	}

	fclose(file_new);
	fclose(file);
	return 0;
}
