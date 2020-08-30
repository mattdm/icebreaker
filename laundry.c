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
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*
*/


#include <SDL.h>
#include <stdlib.h>
#include "icebreaker.h"
#include "globals.h"
#include "laundry.h"
#include "options.h"
#include "fullscreen.h"

static SDL_Rect laundrylist[MAXDIRTY];
static int laundrycount;

void initlaundry()
{
	laundrycount=0;
}


void soil(SDL_Rect r)
{ // makes stuff dirty, of course.
	if (laundrycount<MAXDIRTY) // it's good to have this check here, but
	{                          // since this is the most-used function in
	                           // the whole program, it might be worth removing
	                           // for production builds...
		laundrylist[laundrycount] = r;
		laundrycount++;
	}
	else
	{	
		// fix -- we ought to at least fail gracefully.
		// (clean whole screen, laundrylist)
		// might be worth dynamically calculating the point at which
		// updating everything is more efficient than updating many
		// rects.
		fprintf(stderr, "Too much dirty laundry!\n");
		exit(1);
	}
}

void clean()
{
	int i;
	if (gameflags.isfullscreen)
	{
		for( i = 0; i < laundrycount; i++)
		{ // fix -- sucks to have to do a loop here, but not
		  // easy to avoid.
			laundrylist[i].x += (FULLWIDTH - WIDTH) / 2;
			laundrylist[i].y += FULLTOPMARGIN;
		}
		SDL_UpdateRects(fullscreen, laundrycount, laundrylist);		
	}
	else
	{
		SDL_UpdateRects(screen, laundrycount, laundrylist);
	}
	laundrycount=0;
}

void updateall()
{
	// FIX -- anything that needs to call this should be fixed to
	// use the laundry list properly. then we can remove this
	// function completely.
	
	if (gameflags.isfullscreen)
		SDL_UpdateRect(fullscreen,0,0,0,0);
	else
		SDL_UpdateRect(screen,0,0,0,0);
		
	laundrycount=0;
}
