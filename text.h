/*
* IceBreaker
* Copyright (c) 2000-2002 Matthew Miller <mattdm@mattdm.org>
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

#ifndef TEXT_H
#define TEXT_H

#define CHARHEIGHT 6
#define CHARWIDTH 6
#define BIGCHARHEIGHT 25
#define BIGCHARWIDTH 35
#define CHARSPACE 2

extern int gettextwidth(int size, const char * text);
extern int getletterwidth(int size, const char letter);
extern void puttext(int x, int y, int size, Uint32 color, char * text);
extern void sputtext(SDL_Surface* scr,int x, int y, int size, Uint32 color, char * text);
extern void inittext();

#endif /* TEXT_H */
