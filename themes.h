/*
* IceBreaker
* Copyright (c) 2002-2020 Matthew Miller <mattdm@mattdm.org>
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
* with this program; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#ifndef THEMES_H
#define THEMES_H

typedef struct
{
	Uint32 background;
	Uint32 normaltext;
	Uint8  boardfillminr;
	Uint8  boardfillming;
	Uint8  boardfillminb;
	Uint8  boardfillmaxr;
	Uint8  boardfillmaxg;
	Uint8  boardfillmaxb;
	Uint32 gridline;
	Uint32 gridhighlight;
	Uint32 line1;
	Uint32 line2;
	Uint32 menuhighlight;
	Uint32 gameovertext;
	Uint32 scorescrolltext;
	Uint32 bonusscrolltext;
	Uint32 textentrybox;
	Uint32 textentrytext;
	Uint32 copyrighttext;
	Uint32 spritetransparent; // fix -- this doesn't really belong here

} ThemeColorsType;

extern ThemeColorsType color;

extern SDL_Surface * spriteimage;
extern SDL_Surface * spritemirrorimage;



// you might expect sounds to go here, but in the interest of keeping
// SDL_mixer stuff confined to the sound module, it's done separately

int getthemenames(char*** themenamelist);
void freethemenames(char*** themenamelist,int numthemes);
int getthemenumber(char** themenamelist,int numthemes,char* themename);
int settheme(char* themename);

#endif /* THEMES_H */
