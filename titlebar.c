/*
* IceBreaker
* Copyright (c) 2000-2020 Matthew Miller <mattdm@mattdm.org> and
*   Enrico Tassi <gareuselesinge@infinito.it>
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
#include "icebreaker.h"
#include "globals.h"
#include "options.h"
#include "laundry.h"
#include "text.h"
#include "fullscreen.h"

#define CAPTIONSIZE 3
#define TITLEBORDER 5

void drawtitlebar(const char *title)
{
	SDL_Rect titlebar;
	
	if (gameflags.isfullscreen)
	{
		titlebar.h=CHARHEIGHT*CAPTIONSIZE+TITLEBORDER*2;
		titlebar.w=WIDTH; // gettextwidth(CAPTIONSIZE,title)+TITLEBORDER*2;
		titlebar.x=(FULLWIDTH - WIDTH)/2;
		titlebar.y=FULLTOPMARGIN - titlebar.h - TITLEBORDER*3;
		
		SDL_FillRect(fullscreen,&titlebar,SDL_MapRGB(screen->format,0,0,0));
		sputtext(fullscreen,titlebar.x+TITLEBORDER, titlebar.y+TITLEBORDER, CAPTIONSIZE,SDL_MapRGB(screen->format,192,192,192),(char*)title);
		soil(titlebar);
	}
	
	SDL_WM_SetCaption(title,"IceBreaker");
}
