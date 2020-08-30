/*
* IceBreaker
* Copyright (c) 2000-2002 Matthew Miller <mattdm@mattdm.org>
* except the init_system_cursor function, which was lovingly stolen from
* the SDL documentation
*
* <http://www.mattdm.org/icebreaker/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*
*/



#include <SDL.h>
#include "cursor.h"
#include "icebreaker.h"

static SDL_Cursor* createcursor(const char *image[]);

static SDL_Cursor* cursorarrow;
static SDL_Cursor* cursorhorizontal;
static SDL_Cursor* cursorvertical;
static SDL_Cursor* cursorclick;


/* XPM */
static const char *cursorarrowxpm[] = {
/* width height num_colors chars_per_pixel */
"    32    32        3            1",
/* colors */
"X c #000000",
". c #ffffff",
"  c None",
/* pixels */
"X                               ",
"XX                              ",
"X.X                             ",
"X..X                            ",
"X...X                           ",
"X....X                          ",
"X.....X                         ",
"X......X                        ",
"X.......X                       ",
"X........X                      ",
"X.....XXXXX                     ",
"X..X..X                         ",
"X.X X..X                        ",
"XX  X..X                        ",
"X    X..X                       ",
"     X..X                       ",
"      X..X                      ",
"      X..X                      ",
"       XX                       ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"0,0"
};


/* XPM */
static const char *cursorhorizontalxpm[] = {
/* width height num_colors chars_per_pixel */
"    32    32        3            1",
/* colors */
"X c #000000",
". c #ffffff",
"  c None",
/* pixels */
"        X X                     ",
"       XX XX                    ",
"      X.X X.X                   ",
"     X..X X..X                  ",
"    X...X X...X                 ",
"   X....X X....X                ",
"  X.....XXX.....X               ",
" X...............X              ",
"X.................X             ",
" X...............X              ",
"  X.....XXX.....X               ",
"   X....X X....X                ",
"    X...X X...X                 ",
"     X..X X..X                  ",
"      X.X X.X                   ",
"       XX XX                    ",
"        X X                     ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"9,8"
};

/* XPM */
static const char *cursorverticalxpm[] = {
/* width height num_colors chars_per_pixel */
"    32    32        3            1",
/* colors */
"X c #000000",
". c #ffffff",
"  c None",
/* pixels */
"        X                       ",
"       X.X                      ",
"      X...X                     ",
"     X.....X                    ",
"    X.......X                   ",
"   X.........X                  ",
"  X...........X                 ",
" X.............X                ",
"XXXXXXX...XXXXXXX               ",
"      X...X                     ",
"XXXXXXX...XXXXXXX               ",
" X.............X                ",
"  X...........X                 ",
"   X.........X                  ",
"    X.......X                   ",
"     X.....X                    ",
"      X...X                     ",
"       X.X                      ",
"        X                       ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"8,9"
};

/* XPM */
static const char *cursorclickxpm[] = {
/* width height num_colors chars_per_pixel */
"    32    32        3            1",
/* colors */
"X c #000000",
". c #ffffff",
"  c None",
/* pixels */
" XXXX  XXX   XXX  XXXX  XXX  XX ",
"X....X X.X   X.X X....X X.X X.X ",
"X.XXX  X.X   X.X X.XXX  X.XX.X  ",
"X.X    X.X   X.X X.X    X...X   ",
"X.X    X.X   X.X X.X    X.XX.X  ",
"X.XXX  X.XXX X.X X.XXX  X.X X.X ",
"X....X X...X X.X X....X X.X X.X ",
" XXXX  XXXXX XXX  XXXX  XXX XXX ",
"                                ",
"                                ",
"        XXXXXXX  XXXXX          ",
"        X.....X X.....X         ",
"        XXX.XXX X.XXX.X         ",
"          X.X   X.X X.X         ",
"          X.X   X.X X.X         ",
"          X.X   X.XXX.X         ",
"          X.X   X.....X         ",
"          XXX    XXXXX          ",
"                                ",
"                                ",
" XXXX  XXXXXX  X    XXXX  XXXXXX",
"X....X X....X X.X   X...X X....X",
"X.XXX  XX.XXX X..X  X.XX.X X.XXX",
"X....X  X.X  X.XX.X X...X  X.X  ",
" XXX.X  X.X X.....X X.X.X  X.X  ",
" XXX.X  X.X X.XXX.X X.XX.X X.X  ",
"X....X  X.X X.X X.X X.XX.X X.X  ",
" XXXX   XXX XXX XXX XXX XX XXX  ",
"                                ",
"                                ",
"                                ",
"                                ",
"15,15"
};


static SDL_Cursor* createcursor(const char *image[])
{
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;

	i = -1;
	for ( row=0; row<32; ++row )
	{
		for ( col=0; col<32; ++col ) 
		{
			if ( col % 8 ) 
			{
		        	data[i] <<= 1;
				mask[i] <<= 1;
			} 
			else
			{
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4+row][col])
			{
				case '.':
					data[i] |= 0x01;
					mask[i] |= 0x01;
				break;
				case 'X':
					mask[i] |= 0x01;
				break;
				case ' ':
				break;
			}
		}
	}
	sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

void setcursor(CursorType c)
{
	static CursorType current=CURSORDEFAULT;
	if (c != current)
	{
		switch (c)
		{
			case CURSORDEFAULT: //falls through
			case CURSORARROW:
				SDL_SetCursor(cursorarrow);
			break;
			case CURSORHORIZONTAL:
				SDL_SetCursor(cursorhorizontal);
			break;
			case CURSORVERTICAL:
				SDL_SetCursor(cursorvertical);
			break;
			case CURSORCLICK:
				SDL_SetCursor(cursorclick);
			break;
		}
		current=c;
	}
}

void initcursors(void)
{
	// fix -- add error checking
	cursorarrow=createcursor(cursorarrowxpm);
	cursorhorizontal=createcursor(cursorhorizontalxpm);
	cursorvertical=createcursor(cursorverticalxpm);
	cursorclick=createcursor(cursorclickxpm);
}


void quitcursors(void)
{
	SDL_FreeCursor(cursorarrow);
	SDL_FreeCursor(cursorhorizontal);
	SDL_FreeCursor(cursorvertical);
	SDL_FreeCursor(cursorclick);
}
