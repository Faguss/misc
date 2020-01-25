#include <cstdio>
#include <cstring>
#include <cstdlib>

//https://community.bistudio.com/wiki/FXY_File_Format
struct FxyEntry {
	unsigned short CharCode;
	unsigned short PaaFileNumber;
	unsigned short X,Y;
	unsigned short Width,Height;
} letter;

char FxyEntry_name[][16] = {
	"CharCode",
	"PaaFileNumber",
	"x",
	"y",
	"w",
	"h"
};

const int names_num = sizeof(FxyEntry_name) / sizeof(FxyEntry_name[0]);

char* number_with_space(char *string, unsigned short number, bool add_space)
{
	sprintf(string, "%u", number);

	if (add_space)
		if (number < 10)
			strcat(string, "  ");
		else 
			if (number < 100)
				strcat(string, " ");

	return string;
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("No filename given");
		return 1;
	}

	const int max_filename          = 512;
	char filename_new[max_filename] = "";

	char *path = argv[1];
	FILE *file = fopen(path, "rb");

	if (!file) {
		perror("");
		return 2;
	}

	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

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

	FILE *file_new = fopen(filename_new, "wb");

	if (!file_new) {
		perror("");
		return 3;
	}

	bool first_item      = true;
	char line[512]       = "";
	char description[16] = "";
	char str_number[16]  = "";

	// Check if arma fxy
	int header = 0;
	fread(&header, 4, 1, file);
	fseek(file, header==0x6F464942 ? 8 : 0, SEEK_SET);


	while(ftell(file)<file_size && !ferror(file)) {
		if (convert_to_txt) {
			fread(&letter, sizeof(letter), 1, file);

			if (header == 0x6F464942)
				fseek(file, 2, SEEK_CUR);

			if (letter.CharCode <= 223) {
				switch(letter.CharCode) {
					case 0   : strcpy(description, "space"); break;
					case 97  : strcpy(description, "empty"); break;
					case 99  : strcpy(description, "f with hook"); break;
					case 104 : strcpy(description, "accent"); break;
					case 112 : strcpy(description, "empty"); break;
					case 120 : strcpy(description, "small tilde"); break;
					case 128 : strcpy(description, "non-breaking space"); break;
					default  : sprintf(description, "%c", letter.CharCode+32);
				}

				if (first_item)		
					first_item = false;
				else
					fprintf(file_new, "\r\n");

				fprintf(file_new, "class CharCode%s {", number_with_space(str_number, letter.CharCode, 1));

				for(int i=1; i<names_num; i++) {
					unsigned short *number = &letter.CharCode+i;
					fprintf(file_new, "%s=%s;", FxyEntry_name[i], number_with_space(str_number, *number, i!=1));
					
					if (i < names_num-1)
						fprintf(file_new, " ");
				}

				fprintf(file_new, "};\t// %s", description);
			}
		} else {
			fgets(line, 512, file);		
			
			for(int i=0; i<names_num; i++) {
				char *find = strstr(line, FxyEntry_name[i]);
				char *value;
				
				if (find == NULL)
					break;
					
				if (i == 0)
					value = find + strlen(FxyEntry_name[i]);
				else {
					char *equality = strchr(find, '=');
					
					if (equality == NULL)
						break;
						
					value = equality + 1;
				}

				char *end = strchr(find, i==0 ? '}' : ';');

				if (end == NULL)
					break;

				line[end-line]        = '\0';
				unsigned short number = atoi(value);
				memcpy(&letter.CharCode+i, &number, 2);

				memset(find, ' ', strlen(FxyEntry_name[i]));
				line[end-line] = ' ';

				if (i==names_num-1 && letter.CharCode<=223)
					fwrite(&letter, sizeof(letter), 1, file_new);
			}
		}
	}

	fclose(file_new);
	fclose(file);
	return 0;
}
