/*
* IceBreaker
* Copyright (c) 2002 Matthew Miller <mattdm@mattdm.org>
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
#include <stdio.h>
#include <stdlib.h>
#include "icebreaker.h"

#if DEVELRELEASE

#include "cursor.h"
#include "penguin.h"
#include "line.h"
#include "grid.h"
#include "sound.h"
#include "laundry.h"
#include "globals.h"
#include "status.h"
#include "text.h"
#include "themes.h"
#include "event.h"
#include "fullscreen.h"

static void setupbenchmark(void);

void setupbenchmark()
{
	int x,y;

	setcursor(CURSORARROW);
	SDL_FillRect(screen,NULL,color.background);

	for (x=0;x<WIDTH;x++)
		for (y=0;y<HEIGHT;y++)
		{
			if (x<BORDERLEFT || x>=BORDERRIGHT || y <BORDERTOP || y>=BORDERBOTTOM) 	
				grid[x][y]='X';
			else
				grid[x][y]=' ';
		}
	
	drawgridblocks();

	updateall();
}



int benchmark(void)
{
	int penguincount=MAXPENGUINS;
	int i;
	SDL_Event event;
	SDL_Rect fpsrect;
	char fpstext[20];
	int done = false;
	
	Penguin flock[MAXPENGUINS];
	Uint32 starttime, endtime, tmptime, framecount, tmpframecount;


	setupbenchmark();

	fpsrect.x=BORDERLEFT; fpsrect.y=BOTTOMSTATUSY;
	fpsrect.h=CHARHEIGHT*3;; fpsrect.w=CHARWIDTH*2*20;
	
	for (i=0;i<penguincount;i++)
	{
		flock[i] = createpenguin();
	}

	framecount=0;
	tmpframecount=0;
	starttime=SDL_GetTicks();
	tmptime=starttime;
	do 
	{
		while (pollevent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				done=true;
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				done=true;
			}
			else if (event.type == SDL_KEYUP)
			{
				switch(translatekeyevent(&event))
				{
					case KEYCANCEL:
						done=true;
					break;
					default:
					break;
				}
			}
		}
		
		
		// move (and get old background)
		
		for (i=0;i<penguincount;i+=2)
		{
			soil(flock[i].geom); // mark the penguin's old position as dirty
			movepenguin(&flock[i]);
			soil(flock[i].geom); // mark the penguin's new position as dirty too (it will be soon...)
			savebehindpenguin(&flock[i]);
		}

		
		// actually draw
		for (i=0;i<penguincount;i+=2)
		{
			drawpenguin(&flock[i]);
		}
		
		// update screen
		clean();

		for (i=0;i<penguincount;i+=2)
		{
			erasepenguin(&flock[i]);
		}
		
		if (SDL_GetTicks() >= tmptime+1000)
		{
			snprintf(fpstext,20,"FPS: %.2f",((framecount-tmpframecount)*1000.0)/(SDL_GetTicks()-tmptime));
			tmptime=SDL_GetTicks();
			tmpframecount=framecount;
			SDL_FillRect(screen,&fpsrect,color.background);
			puttext(fpsrect.x,fpsrect.y+3,2,color.normaltext,fpstext);	
			soil(fpsrect);
		}
		framecount++;
	
	} while (!done);
	endtime=SDL_GetTicks();
	if (endtime>starttime)
		printf("Overall: %.2f frames per second.\n", (framecount*1000.0)/(endtime-starttime) );

	clean();

	while (penguincount)
	{	
		penguincount--;
		deletepenguin(&flock[penguincount]);
	}

	return(false);
}

#endif /* DEVELRELEASE */
