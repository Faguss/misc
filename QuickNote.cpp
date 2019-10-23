//QuickNote application for PocketBook Inkpad 3 by Faguss (ofp-faguss.com) 31.07.2018




	// Headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inkview.h"




	// Global variables
ifont* normal_font = NULL;
ifont* small_font  = NULL;

int SW = 0;	// screen width
int SH = 0; // screen height

const int TEXT_BUFFER_SIZE = 1024 * 1024;
static char text[TEXT_BUFFER_SIZE];
int TEXT_X = 0;
int TEXT_Y = 0;
int TEXT_W = 0;
int TEXT_H = 0;

int BUTTON_ROWS    = 6;
int BUTTON_COLUMNS = 10;
int BUTTON_X       = 0;
int BUTTON_Y       = 0;
int BUTTON_W       = 0;
int BUTTON_H       = 0;
bool SHIFT         = false;

const int KEY_BUFFER_SIZE  = 10;
const char keyboard_buttons[][KEY_BUFFER_SIZE] = {
	"`", "-", "=", "[", "]", "\\", ";", "BACKSPACE", "EXIT",
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
	"q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
	"a", "s", "d", "f", "g", "h", "j", "k", "l", "'",
	"z", "x", "c", "v", "b", "n", "m", ",", ".", "/",
	"SHIFT", " ", "ENTER",
};

const int number_of_keyboard_buttons = sizeof(keyboard_buttons) / sizeof(keyboard_buttons[0]);
int key_x[number_of_keyboard_buttons];
int key_y[number_of_keyboard_buttons];
int key_w[number_of_keyboard_buttons];
int key_h[number_of_keyboard_buttons];




	// Functions

// Replace a character if SHIFT key was pressed
void shift_key(char *current_key) 
{
	if (SHIFT  &&  strlen(current_key)==1) {
		char key = current_key[0];		
	
		switch(key) {
			case '`' : key='~'; break;
			case '1' : key='!'; break;
			case '2' : key='@'; break;
			case '3' : key='#'; break;
			case '4' : key='$'; break;
			case '5' : key='%'; break;
			case '6' : key='^'; break;
			case '7' : key='&'; break;
			case '8' : key='*'; break;
			case '9' : key='('; break;
			case '0' : key=')'; break;
			case '-' : key='_'; break;
			case '=' : key='+'; break;
			case '[' : key='{'; break;
			case ']' : key='}'; break;
			case '\\': key='|'; break;
			case ';' : key=':'; break;
			case '\'': key='"'; break;
			case ',' : key='<'; break;
			case '.' : key='>'; break;
			case '/' : key='?'; break;
			default  : {
				if (key>=97 && key<=122)
					key-=32;
			}; break;
		}

		current_key[0] = key;
	}
}



// Display rectangles with letters in them
void draw_keyboard() 
{
	FillArea(BUTTON_X, BUTTON_Y, SW, SH - BUTTON_Y, WHITE);	

	int column = 0;
	int row    = 0;
	bool small = 0;

	for (int i=0; i<number_of_keyboard_buttons; i++) {
		key_x[i] = column>0 ? key_x[i-1] + key_w[i-1] : BUTTON_X;
		key_y[i] = BUTTON_Y + BUTTON_H * row;
		key_w[i] = BUTTON_W;
		key_h[i] = BUTTON_H;

		if (row==0 && column==7)	// backspace
			key_w[i] *= 2;

		if (row==5 && column==1)	// space
			key_w[i] *= 6;

		if (row==5 && (column==0 || column==2))	// shift, enter
			key_w[i] *= 2;

		char current_key[KEY_BUFFER_SIZE];
		strcpy(current_key, keyboard_buttons[i]);
		shift_key(current_key);
		const char *text_to_add = current_key;

		small = strlen(text_to_add) > 2;
		if (small)
			SetFont(small_font, DGRAY);
		else
			SetFont(normal_font, DGRAY);	

		DrawRect(key_x[i], key_y[i], key_w[i], key_h[i], LGRAY);
		DrawString(
			key_x[i] + key_w[i] / 2 - StringWidth(text_to_add) / 2, 
			key_y[i] + key_h[i] / 2 - ((small ? small_font : normal_font)->height) / 2 , 
			text_to_add
		);

		column++;

		if (column >= BUTTON_COLUMNS  ||  (row==0 && column>8)) {
			column = 0;
			row++;
		}
	}
}



// Display text area
void draw_text() 
{
	SetFont(small_font, BLACK);
	FillArea(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, WHITE);
	DrawTextRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, text, ALIGN_LEFT | VALIGN_TOP);
};



// Main program
int main_handler(int type, int par1, int par2)
{
	switch(type)
	{
	    case EVT_INIT:
	    {
        	SetPanelType(0);
			normal_font = OpenFont(DEFAULTFONT, BUTTON_H - BUTTON_H/3  , 1);
			small_font  = OpenFont(DEFAULTFONT, BUTTON_H - BUTTON_H/1.5, 1);
	    }
	    break;


	    case EVT_SHOW:
		{
			draw_text();
			draw_keyboard();
			FullUpdate();
		}
		break;


		case EVT_POINTERUP: 
		{
			int x = par1;
			int y = par2;

			if (y > BUTTON_Y) {
				for (int i=0; i<number_of_keyboard_buttons; i++) {
					if (x > key_x[i]  &&  y > key_y[i]  &&  x < key_x[i]+key_w[i]  &&  y < key_y[i]+key_h[i]) {
						int length = strlen(text);
												
						if (strcmp(keyboard_buttons[i],"EXIT") == 0) {
							CloseApp();
						} else 
						if (strcmp(keyboard_buttons[i],"SHIFT") == 0) {
							SHIFT = !SHIFT;
							draw_keyboard();
							PartialUpdate(BUTTON_X, BUTTON_Y, SW, SH - BUTTON_Y);
						} else {
							if (strcmp(keyboard_buttons[i],"BACKSPACE") == 0) {
								if (length > 0)
									text[length-1] = '\0';
							} else {
								if (length < TEXT_BUFFER_SIZE-1) {
									if (strcmp(keyboard_buttons[i],"ENTER") == 0)
										strcat(text,"\n");
									else {
										char current_key[KEY_BUFFER_SIZE];
										strcpy(current_key, keyboard_buttons[i]);
										shift_key(current_key);										
										strcat(text, current_key);
									}
								}
							}

							draw_text();
							PartialUpdate(TEXT_X, TEXT_Y, TEXT_W, TEXT_H);
						}
					}
				}				
			} else
				FullUpdate();
		}
		break;

	    case EVT_EXIT: 
		{
			FILE *f = fopen("//mnt/ext1/.QuickNote.txt", "w");
			if (f) {
				fprintf(f, "%s", text);
				fclose(f);
			};

			CloseFont(normal_font); 
			CloseFont(small_font); 
			normal_font = NULL;
			small_font  = NULL;
		};
	    break;

	    default:
	    	break;
	}

	return 0;

}




// Initialization
int main()
{
	FILE *f = fopen("//mnt/ext1/.QuickNote.txt", "r");
	if (f) {
		fread(text, sizeof(char), TEXT_BUFFER_SIZE-1, f);
		fclose(f);
	};

	OpenScreen();
	SW       = ScreenWidth();
	SH       = ScreenHeight();
	BUTTON_W = SW / BUTTON_COLUMNS;
	BUTTON_H = BUTTON_W - 5;
	BUTTON_Y = SH - BUTTON_H * BUTTON_ROWS;
	TEXT_W   = SW;
	TEXT_H   = BUTTON_Y;

	InkViewMain(main_handler);
	return 0;
}
