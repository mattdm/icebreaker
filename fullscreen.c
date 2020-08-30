/*
* IceBreaker
* Copyright (c) 2002 Matthew Miller <mattdm@mattdm.org> and
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
#include <stdlib.h>
#include "icebreaker.h"
#include "globals.h"
#include "laundry.h"
#include "options.h"
#include "text.h"
#include "titlebar.h"

int makefullscreen()
{
	SDL_Surface* fullscreensave = NULL;
	

	//for window manager caption -- the title bar
	char *curcaptiontitle=NULL,*curcaptionicon=NULL;
	char newcaptiontitle[35];


	
	if (screen!=NULL)
	{
		clean(); // very important!
		
		fullscreensave=SDL_CreateRGBSurface(SDL_SWSURFACE,WIDTH,HEIGHT,screen->format->BitsPerPixel,0,0,0,0);
		SDL_BlitSurface(screen, NULL, fullscreensave, NULL);
	
		// the resolution switch is ugly if we don't do this, since
		// sometimes the window gets resized a second before the mode
		// switch. ugh.
		SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format, 0,  0,  0));
		SDL_UpdateRect(screen,0,0,0,0);
	}

	fullscreen = SDL_SetVideoMode(FULLWIDTH, FULLHEIGHT, VIDEODEPTH, SDL_SWSURFACE | SDL_FULLSCREEN );
	if (fullscreen == NULL)
	{
		// FIX -- if this happens, don't die: just stay in windowed
		// mode and return failure (and change calling code to
		// deal with the return value).
		fprintf(stderr, "Couldn't switch to full screen mode.\n"
		                "SDL error: "
		                "%s\n\n", SDL_GetError());
		exit(1);
	}	
	
	screen = NULL;
	
	// very clever idea from Enrico
	screen = SDL_CreateRGBSurfaceFrom(fullscreen->pixels +
	                fullscreen->format->BytesPerPixel * FULLLEFTMARGIN +
	                FULLTOPMARGIN*fullscreen->format->BytesPerPixel *  FULLWIDTH,
	                WIDTH, HEIGHT, VIDEODEPTH,
	                FULLWIDTH * fullscreen->format->BytesPerPixel,0,0,0,0);

	if (screen == NULL)
	{
		// fix -- same as above -- try to recover gracefully if this
		// happens
		fprintf(stderr, "Couldn't access full screen surface. That's not good.\n"
				"SDL error: "
				"%s\n\n", SDL_GetError());
				exit(1);
	}
	gameflags.isfullscreen=true;
	
	// if we wanted to something interesting in the border in full-screen
	// mode, here would be the place.
	SDL_FillRect(fullscreen,NULL,SDL_MapRGB(screen->format, 0,  0,  0));
	
	if (fullscreensave!=NULL)
		SDL_BlitSurface(fullscreensave, NULL, screen, NULL);

	// Set Window Caption For some odd reason, you can't feed the result
	// of GetCaption directly back into SetCaption. So we have to do this.
	SDL_WM_GetCaption(&curcaptiontitle, &curcaptionicon);
	if (curcaptiontitle != NULL && curcaptionicon != NULL)
	{
		snprintf(newcaptiontitle,35,"%s",curcaptiontitle);
		drawtitlebar(newcaptiontitle);
	}

	SDL_UpdateRect(fullscreen,0,0,0,0);
	
	SDL_FreeSurface(fullscreensave);
	
	
	// hmmm -- we might need to recalculate the colors if the video
	// depth happens to change when switching to fullscreen. could 
	// happen....
	
	return 0;
}

int makewindowed()
{
	SDL_Surface * fullscreensave=SDL_CreateRGBSurface(SDL_SWSURFACE,WIDTH,HEIGHT,screen->format->BitsPerPixel,0,0,0,0);

	clean(); // very important!
	
	SDL_BlitSurface(screen, NULL, fullscreensave, NULL);
	
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, VIDEODEPTH, SDL_SWSURFACE);
	if (screen == NULL)
	{
		fprintf(stderr, "Couldn't switch to windowed screen mode.\n"
		                "SDL error: "
		                "%s\n\n", SDL_GetError());
		exit(1);
	}	
	gameflags.isfullscreen=false;
	
	SDL_BlitSurface(fullscreensave, NULL, screen, NULL);

	SDL_UpdateRect(screen,0,0,0,0);

	SDL_FreeSurface(fullscreensave);

	return 0;
}
