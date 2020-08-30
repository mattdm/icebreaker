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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#ifndef __MINGW32__
	#include <pwd.h>
#endif
#include <string.h>
#include <sys/types.h>
              

#include "icebreaker.h"
#include "penguin.h"
#include "line.h"
#include "sound.h"
#include "globals.h"
#include "grid.h"
#include "level.h"
#include "intro.h"
#include "text.h"
#include "transition.h"
#include "hiscore.h"
#include "dialog.h"
#include "options.h"
#include "fullscreen.h"
#include "cursor.h"
#include "themes.h"
#include "titlebar.h"
#include "benchmark.h"
#include "misc.h"

// global
SDL_Surface* screen;
SDL_Surface* fullscreen;

char username[50]; // FIX -- move this into the options struct?
char homedir[255];

SDL_Surface * penguinicon;

// functions

int setup(void);
void cleanup(void);

/************************************************************************/

int setup(void)
{
	struct passwd * userinfo;
	int newuser=false;
	
	srandom(time(NULL)+getpid());	
	
	//stupid buffers
	setvbuf(stdout,(char *)NULL, _IOLBF, 0);
	
	userinfo = getpwuid(getuid()); // FIX -- make this part of the options struct; and maybe save in options file
	snprintf(username,50,"%s",userinfo->pw_name); // not like it's gonna be fifty characters. but y'know. note: gets chopped to fit in gethighusername().
	snprintf(homedir,255,"%s",userinfo->pw_dir); // fix - use OS define for length

	inithiscores();
	newuser=readoptions();		

	if (commandline.fullscreen==FULLSCREENOFF)
		options.fullscreen=FULLSCREENOFF;	
	else if (commandline.fullscreen==FULLSCREENON && options.fullscreen==FULLSCREENOFF)
		options.fullscreen=FULLSCREENON;


	if (SDL_Init(SDL_INIT_VIDEO))
	{
		fprintf(stderr, "Hey. We're gonna need some graphics.\n"
		                "SDL error: " 
		                "%s\n\n", SDL_GetError());
		exit(1);
	}
	
	
	atexit(cleanup);


	penguinicon = SDL_LoadBMP(DATAPREFIX "/" PENGUINICONFILE);
	if (penguinicon==NULL) fprintf(stderr, "Icon not loaded!\n\n*** IceBreaker probably wasn't installed properly. ***\n\n");
	SDL_WM_SetIcon(penguinicon,NULL);
	
	if (options.fullscreen==FULLSCREENOFF)
	{
		// gotta do this right away or else we risk getting an ugly "SDL_App"
		// in the titlebar for a few milliseconds -- can't have that!
		SDL_WM_SetCaption("IceBreaker","IceBreaker");
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, VIDEODEPTH, SDL_SWSURFACE);

	}
	else
	{
		screen = NULL;
		makefullscreen();
	}

	if (screen == NULL)
	{
		fprintf(stderr, "Help! Couldn't get a window.\n"
		                "SDL error: " 
		                "%s\n\n", SDL_GetError());
		exit(1);
	}

	initsound();

	inittext();

	initgrid();

	initcursors();

	if (strlen(commandline.theme)>0)
		settheme(commandline.theme);
	else
		settheme(options.theme);

	return newuser;
}

void cleanup()
{
	quitgrid();
	quitsound();
	quitcursors();
	SDL_Quit();
	writeoptions();
	writedelayedhiscores();
}



int main(int argc,char** argv)
{
	int done = false;
	int level=0;
	ScoreSheet levelscore;
	long totalscore=0;
	char windowtitle[35];
	LevelExitType levelresult;
	int newuser=false;
	int rc=0;

	#ifdef NEEDCHANGETOARGV0PATH
	changetoargv0path(argv[0]);
	#endif

	rc=parsecommandline(argc,argv);
	if (rc) return rc;
			
	newuser=setup();
	
	drawtitlebar("IceBreaker");

#if DEVELRELEASE
	if (gameflags.benchmarkmode)
	{
		drawtitlebar("IceBreaker -- Benchmark mode");
		rc=benchmark();
		return rc;
	}
#endif /* DEVELRELEASE */

	done=intro();	

	if (!done && newuser)
	{ // no options file; using the default
		setcursor(CURSORCLICK);
		if (popuphelp()==POPUPQUITGAME) done=true;
		setcursor(CURSORARROW);
	}
	
		
 	while(!done)
	{
		level++;
		if (level>=MAXPENGUINS) level=MAXPENGUINS-1;

		switch (options.difficulty)
		{
			case NORMAL:
				snprintf(windowtitle,35,"IceBreaker -- Level %d",level);
			break;
			case HARD:
				snprintf(windowtitle,35,"IceBreaker -- Level %d (Hard)",level);
			break;
			case EASY:
				snprintf(windowtitle,35,"IceBreaker -- Level %d (Easy)",level);
			break;
		}			
		drawtitlebar(windowtitle);
		
		if (!strcmp(commandline.theme,"random") || (strlen(commandline.theme)==0 && !strcmp(options.theme,"random")))
			settheme("random");
		
		levelresult=playlevel(level,totalscore,&levelscore);
		
  		drawtitlebar("IceBreaker");
		
		totalscore+= levelscore.basescore + levelscore.clearbonus + levelscore.lifebonus;
		
		switch (levelresult)
		{
			case QUIT:
				done=true;
			break;
			case DEAD:
				done=gameover(totalscore);
			// falls through
			case ZERO:
				// hooray! modifying the index variable in the loop!
				// good coding practice at its finest!
				level=0; 
				totalscore=0; 
			break;
			case ERROR:
				fprintf(stderr,"Level error -- this should never happen.\n");
			break;
			case PASS:			
				// level completed successfully
				done=intermission(&levelscore,level+1);
			break;
		}

	}
	
	return rc;
}
