/*
* IceBreaker
* Copyright (c) 2000-2002 Matthew Miller <mattdm@mattdm.org> and
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
* with this program; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

/* special thanks to Enrico for the grid coloring stuff for themes */
/* and for the vastly improved circular scrolling code */


#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "icebreaker.h"
#include "cursor.h"
#include "penguin.h"
#include "line.h"
#include "grid.h"
#include "sound.h"
#include "laundry.h"
#include "globals.h"
#include "level.h"
#include "status.h"
#include "text.h"
#include "hiscore.h"
#include "themes.h"
#include "event.h"
#include "fullscreen.h"
#include "options.h"

#define LXPOS(x) (BORDERLEFT+(BLOCKWIDTH*(x)))
#define LYPOS(y) (BORDERTOP+(BLOCKHEIGHT*(y)))

static void setupintro(void);
void circularblit(SDL_Surface* scoresurface,SDL_Rect* scrollrect,Uint16 hiscorescroll);
SDL_Surface* createscoresurface();

void setupintro()
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
				grid[x][y]='w';
		}
	
	/// Here's the fun part where we set up the letters.
	// I
	markgrid(LXPOS(1),LYPOS(2),BLOCKWIDTH*3,BLOCKHEIGHT  ,' ');
	markgrid(LXPOS(1),LYPOS(7),BLOCKWIDTH*3,BLOCKHEIGHT  ,' ');
	markgrid(LXPOS(2),LYPOS(2),BLOCKWIDTH  ,BLOCKHEIGHT*6,' ');
	
	//C
	markgrid(LXPOS(5),LYPOS(2),BLOCKWIDTH*4,BLOCKHEIGHT  ,' ');
	markgrid(LXPOS(5),LYPOS(3),BLOCKWIDTH  ,BLOCKHEIGHT*5,' ');	
	markgrid(LXPOS(6),LYPOS(7),BLOCKWIDTH*3,BLOCKHEIGHT,  ' ');
	
	//E
	markgrid(LXPOS(10),LYPOS(2),BLOCKWIDTH*4,BLOCKHEIGHT  ,' ');
	markgrid(LXPOS(10),LYPOS(3),BLOCKWIDTH  ,BLOCKHEIGHT*5,' ');	
	markgrid(LXPOS(11),LYPOS(7),BLOCKWIDTH*3,BLOCKHEIGHT,  ' ');
	markgrid(LXPOS(11),LYPOS(4),BLOCKWIDTH*2,BLOCKHEIGHT  ,' ');

	drawgridblocks();

	updateall();
}



int intro(void)
{
	int penguincount=0;
	int frozen=true;
	int i;
	int x,y;
	int letterstep=0;
	int hiscorescroll=0;
	int hiscorestep=0;
	int linedone1=false; int linedone2=false;

	SDL_Rect labelrect;
	SDL_Rect scrollrect;
	SDL_Rect bigrect;
	SDL_Rect rightmarginrect;
	SDL_Rect leftmarginrect;
#ifdef DEVELRELEASE
	SDL_Rect develdisclaimerrect;
#endif
	
	int done = false;
	
	Penguin flock[100];
	Penguin tux;
	
	SDL_Event event;

	SDL_Surface* scoresurface;

	setupintro();
	
#ifdef DEVELRELEASE
	develdisclaimerrect.x=BORDERLEFT-3;
	develdisclaimerrect.y=MARGINTOP+PLAYHEIGHT+CHARHEIGHT*2;
	develdisclaimerrect.h=CHARHEIGHT*3;
	develdisclaimerrect.w=WIDTH-develdisclaimerrect.x;
#endif		
					
	labelrect.x=BORDERLEFT;
	labelrect.y=BORDERTOP-(4*CHARHEIGHT);
	labelrect.h=CHARHEIGHT*2;
	labelrect.w=CHARWIDTH*21;
			
	scrollrect.x=BORDERLEFT+CHARWIDTH*21;
	scrollrect.y=labelrect.y;
	scrollrect.w=BORDERRIGHT-(BORDERLEFT+CHARWIDTH*21);
	scrollrect.h=labelrect.h;

	rightmarginrect.x=BORDERRIGHT;
	rightmarginrect.y=labelrect.y;
	rightmarginrect.w=MARGINRIGHT;
	rightmarginrect.h=labelrect.h;
	
	leftmarginrect.x=0;
	leftmarginrect.y=labelrect.y;
	leftmarginrect.w=BORDERLEFT;
	leftmarginrect.h=labelrect.h;
	
	bigrect.x=labelrect.x;
	bigrect.y=labelrect.y;
	bigrect.w=WIDTH-(BORDERLEFT);
	bigrect.h=labelrect.h;

	hiscorescroll=0;

	puttext(labelrect.x,labelrect.y,2,color.normaltext,"HIGH SCORES:");
	soil(labelrect);

#ifdef DEVELRELEASE
	puttext(develdisclaimerrect.x,develdisclaimerrect.y,1,color.copyrighttext,"This is a development (beta) release meant for bug-testing only. Please check");
	puttext(develdisclaimerrect.x+3,develdisclaimerrect.y+CHARHEIGHT*1.5,1,color.copyrighttext,"<http://www.mattdm.org/icebreaker/> for the latest stable release. Thanks!");
	soil(develdisclaimerrect);
#endif 

	line1=createline(1);
	line2=createline(2);

	tux = createpenguin();
	
	// create highscore surface
	scoresurface = createscoresurface();

	do 
	{
		while (pollevent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				return(true);
				// fix -- should delete penguins here
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
						if (gameflags.isfullscreen) makewindowed();
					break;
					case KEYSWITCHLINE: // falls through
					case KEYMENU:      // yep
					case KEYSTARTLINE: // start game
						if (letterstep>1) done=true; // kludge to keep from catching enter keystroke which launched app
					default:
					break;
				}
			}
		}
		if (letterstep < 10)
		{	
			switch (letterstep)
			{
				// I
				case 0:
					if (!linedone1 && !line1.on) startline(&line1,LEFT,LXPOS(2),LYPOS(2));
					if (!linedone2 && !line2.on) startline(&line2,RIGHT,LXPOS(2),LYPOS(2));
				break;
				case 1:
					if (!linedone1 && !line1.on) startline(&line1,LEFT,LXPOS(2),LYPOS(7));
					if (!linedone2 && !line2.on) startline(&line2,RIGHT,LXPOS(2),LYPOS(7));
				break;
				case 2:
					if (!linedone1 && !line1.on) startline(&line1,UP,LXPOS(2),LYPOS(5));
					if (!linedone2 && !line2.on) startline(&line2,DOWN,LXPOS(2),LYPOS(5));
				break;
				// C
				case 3:
					if (!linedone1 && !line1.on) startline(&line1,LEFT,LXPOS(7),LYPOS(2));
					if (!linedone2 && !line2.on) startline(&line2,RIGHT,LXPOS(7),LYPOS(2));
				break;
				case 4:
					if (!linedone1 && !line1.on) startline(&line1,UP,LXPOS(5),LYPOS(5));
					if (!linedone2 && !line2.on) startline(&line2,DOWN,LXPOS(5),LYPOS(5));
				break;
				case 5:
					if (!linedone1 && !line1.on) startline(&line1,LEFT,LXPOS(7),LYPOS(7));
					if (!linedone2 && !line2.on) startline(&line2,RIGHT,LXPOS(7),LYPOS(7));
				break;
				// E
				case 6:
					if (!linedone1 && !line1.on) startline(&line1,LEFT,LXPOS(11),LYPOS(2));
					if (!linedone2 && !line2.on) startline(&line2,RIGHT,LXPOS(11),LYPOS(2));
				break;
				case 7:
					if (!linedone1 && !line1.on) startline(&line1,UP,LXPOS(10),LYPOS(5));
					if (!linedone2 && !line2.on) startline(&line2,DOWN,LXPOS(10),LYPOS(5));
				break;
				case 8:
					if (!linedone1 && !line1.on) startline(&line1,LEFT,LXPOS(13),LYPOS(7));
					if (!linedone2 && !line2.on) startline(&line2,RIGHT,LXPOS(13),LYPOS(7));
				break;
				case 9:
					if (!linedone1 && !line1.on) startline(&line1,LEFT,LXPOS(12),LYPOS(4));
					if (!linedone2 && !line2.on) startline(&line2,RIGHT,LXPOS(12),LYPOS(4));
				break;
			}
				
		}
		else if (letterstep==10)
		{
			for (x=0;x<WIDTH;x++)
				for (y=0;y<HEIGHT;y++)
					if (grid[x][y]=='w')
					{
						grid[x][y]=' ';
					}
					// FIX: known bug -- if a line stops partway through a grid cell,
					// the space left over is not cleared properly later. this isn't 
					// an issue in the game, as lines aways stop on cell boundaries, but
					// sometimes shows up in the intro. this is a kludge
					// which should fix it.
					else if (((x-BORDERLEFT) % BLOCKWIDTH)==0 && ((y-BORDERTOP) % BLOCKHEIGHT)==0)
					{
						if (grid[x+BLOCKWIDTH-1][y+BLOCKHEIGHT-1]==' ')
							grid[x][y]=' ';
					}
			letterstep++;
		}
		else if (letterstep==11)
		{
			//B
			flock[penguincount]=createpenguinxy(LXPOS(0),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(0),LYPOS(12)); penguincount++; 
			flock[penguincount]=createpenguinxy(LXPOS(0),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(0),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(0),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(0),LYPOS(16)); penguincount++;

			flock[penguincount]=createpenguinxy(LXPOS(1),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(2),LYPOS(11)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(1),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(2),LYPOS(13)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(1),LYPOS(16)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(2),LYPOS(16)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(3),LYPOS(12)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(3),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(3),LYPOS(15)); penguincount++;
			
			//R
			flock[penguincount]=createpenguinxy(LXPOS(5),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(5),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(5),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(5),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(5),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(5),LYPOS(16)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(6),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(7),LYPOS(11)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(6),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(7),LYPOS(13)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(8),LYPOS(12)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(8),LYPOS(14)); penguincount++;
 			flock[penguincount]=createpenguinxy(LXPOS(8),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(8),LYPOS(16)); penguincount++;
			
			//E
			flock[penguincount]=createpenguinxy(LXPOS(10),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(10),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(10),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(10),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(10),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(10),LYPOS(16)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(11),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(12),LYPOS(11)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(11),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(12),LYPOS(13)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(11),LYPOS(16)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(12),LYPOS(16)); penguincount++;
			
			//A
			flock[penguincount]=createpenguinxy(LXPOS(14),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(14),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(14),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(14),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(14),LYPOS(16)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(17),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(17),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(17),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(17),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(17),LYPOS(16)); penguincount++;

			flock[penguincount]=createpenguinxy(LXPOS(15),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(16),LYPOS(11)); penguincount++;

			flock[penguincount]=createpenguinxy(LXPOS(15),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(16),LYPOS(13)); penguincount++;

			//K
			flock[penguincount]=createpenguinxy(LXPOS(19),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(19),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(19),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(19),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(19),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(19),LYPOS(16)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(20),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(21),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(22),LYPOS(11)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(21),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(22),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(22),LYPOS(16)); penguincount++;

			//E
			flock[penguincount]=createpenguinxy(LXPOS(24),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(24),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(24),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(24),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(24),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(24),LYPOS(16)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(25),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(26),LYPOS(11)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(25),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(26),LYPOS(13)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(25),LYPOS(16)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(26),LYPOS(16)); penguincount++;

			//R
			flock[penguincount]=createpenguinxy(LXPOS(28),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(28),LYPOS(12)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(28),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(28),LYPOS(14)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(28),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(28),LYPOS(16)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(29),LYPOS(11)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(30),LYPOS(11)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(29),LYPOS(13)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(30),LYPOS(13)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(31),LYPOS(12)); penguincount++;
			
			flock[penguincount]=createpenguinxy(LXPOS(31),LYPOS(14)); penguincount++;
 			flock[penguincount]=createpenguinxy(LXPOS(31),LYPOS(15)); penguincount++;
			flock[penguincount]=createpenguinxy(LXPOS(31),LYPOS(16)); penguincount++;

			if (penguincount>=MAXPENGUINS) { fprintf(stderr,"Too many penguins!\n"); }

			for (i=0;i<penguincount;i++)
			{
				savebehindpenguin(&flock[i]);
				markgrid(flock[i].geom.x,flock[i].geom.y,BLOCKWIDTH,BLOCKHEIGHT,'*');	
				drawpenguin(&flock[i]);
				soil(flock[i].geom);
			}
			
			
			letterstep++;
		}
		else if (letterstep<300)
		{
			letterstep++;
		}
		else if (letterstep==300)
		{
			frozen=false;
			for (i=0;i<penguincount;i++)
			{
				markgrid(flock[i].geom.x,flock[i].geom.y,BLOCKWIDTH,BLOCKHEIGHT,' ');
				erasepenguin(&flock[i]);
				soil(flock[i].geom);
			}
			letterstep++;
			setcursor(CURSORCLICK);
		}
		else if (letterstep<400)
		{
			letterstep++;
		}
		else if (letterstep==400)
		{
			if (penguincount)
			{
				penguincount--;
				markgrid(flock[penguincount].geom.x,flock[penguincount].geom.y,BLOCKWIDTH,BLOCKHEIGHT,' ');
				erasepenguin(&flock[penguincount]);
				soil(flock[penguincount].geom);
				deletepenguin(&flock[penguincount]);
			}
			else
			{
				letterstep++;
			}
		}
		else if (letterstep<800)
		{
			if (!line1.on && !line2.on)
			{
				x=LXPOS(random() % 32);
				y=LYPOS(random() % 20);
				if (random()%2)
				{
					startline(&line1,UP,x,y);
					startline(&line2,DOWN,x,y);
				}
				else
				{
					startline(&line1,LEFT,x,y);
					startline(&line2,RIGHT,x,y);
				}

			}			
		}
		//else if (letterstep<2000)
		//{
		//	letterstep++;
		//}
		//else
		//{
		//	done=true;
		//}

		
		hiscorestep--;
		if (hiscorestep<0)
		{
			circularblit(scoresurface,&scrollrect,hiscorescroll);
			soil(scrollrect);
			hiscorescroll++;
			hiscorestep=2;
		}
		if (hiscorescroll >= scoresurface->w)
		{
			hiscorescroll=0;
		}
		
		// move split-lines
		if (line1.on) 
		{
			if (moveline(&line1))
			{
				linedone1=true;
			}
			if (line1.dead) // need a second check because moveline could change this
			{
				//no sound in intro?
				//playsound(SNDBREAK);
				killline(&line1);
				linedone1=false;
			}
		}
		if (line2.on)
		{
		
			if (moveline(&line2))
			{
				linedone2=true;
				
			}
			if (line2.dead)
			{ 
				// no sound in intro?
				//playsound(SNDBREAK);
				killline(&line2);
				linedone2=false;
			}
		}
		if (linedone1 && linedone2) { letterstep++; linedone1=false; linedone2=false; }
		
	
		// move (and get old background)
		
		if (!frozen)
		{
			for (i=0;i<penguincount;i+=2)
			{
				soil(flock[i].geom); // mark the penguin's old position as dirty
				movepenguin(&flock[i]);
				soil(flock[i].geom); // mark the penguin's new position as dirty too (it will be soon...)
				savebehindpenguin(&flock[i]);
			}
		}
		soil(tux.geom); // mark the penguin's old position as dirty
		movepenguin(&tux);
		soil(tux.geom); // mark the penguin's new position as dirty too (it will be soon...)
		savebehindpenguin(&tux);
		
		// actually draw
		if (!frozen)
		{
			for (i=0;i<penguincount;i+=2)
			{
				drawpenguin(&flock[i]);
			}
		}
		drawpenguin(&tux);
		
		// update screen
		clean();

		// clear for next update
		if (!frozen)
		{
			for (i=0;i<penguincount;i+=2)
			{
				erasepenguin(&flock[i]);
			}
		}
		erasepenguin(&tux);
		
		SDL_Delay(10);
		
	} while (!done);

	clean();

	while (penguincount)
	{	
		penguincount--;
		deletepenguin(&flock[penguincount]);
	}
	deletepenguin(&tux);

	// destroy highscore surface
	SDL_FreeSurface(scoresurface);
	
	return(false);
}

#define EXTRASCROLLSCORESPACE 30
// FIX -- the max username length is hardcoded at 50 in highscore.c, and as
// 12 in dialog.c. The 12 is so it fits in the dialog box nicely, and should
// be plenty long, but it's good to keep the buffer bigger. Anyway, the value
// in highscore.c should be a define, and this should be that-define + some 
// reasonable value for the width of the score.
#define SCOREMAXLEN 70

SDL_Surface* createscoresurface()
{
	SDL_Surface* tmpsurface;
	char scoretext[SCOREMAXLEN];
	int i,len;

	//caculate needed len
	len=0;
	for( i = 0 ; i < HISCORENUM ; i++)
	{
		snprintf(scoretext,SCOREMAXLEN,"#%d. %s: %ld",i+1,hiscorename[i],hiscoreval[i]);
		len += gettextwidth(2,scoretext);
	}

	len += (HISCORENUM+1) * EXTRASCROLLSCORESPACE;

	//alloc surface
	tmpsurface = SDL_CreateRGBSurface(SDL_SWSURFACE,len,CHARHEIGHT*2,VIDEODEPTH,0,0,0,0);

	// fill surface
	SDL_FillRect(tmpsurface,NULL,color.background);
	len=0;
	for( i = 0 ; i < HISCORENUM ; i++)
	{
		snprintf(scoretext,SCOREMAXLEN,"#%d. %s: %ld",i+1,hiscorename[i],hiscoreval[i]);
		sputtext(tmpsurface,len,0,2,color.normaltext,scoretext);
		len += gettextwidth(2,scoretext) + EXTRASCROLLSCORESPACE;
	}

	return tmpsurface;
}

// this assumes that scoresurface->w > scrollrect->w
void circularblit(SDL_Surface* scoresurface,SDL_Rect* scrollrect,Uint16 hiscorescroll)
{
	SDL_Rect from,to;
	Sint32 len;

	//calculate fiirst part w
	if( scoresurface->w - hiscorescroll < scrollrect->w)
		len = scoresurface->w - hiscorescroll;
	else
		len = scrollrect->w;

	// draw first part to scoresurface
	from.x=hiscorescroll;
	from.y=0;
	from.w=len;
	from.h=scoresurface->h;

	to.x=scrollrect->x;
	to.y=scrollrect->y;
	to.w=from.w;
	to.h=from.h;
	
	SDL_BlitSurface(scoresurface,&from,screen,&to);

	//if needed, draw second part to scoresurface
	if(len < scrollrect->w)
	{
	       	from.x=0;
	        from.y=0;
	        from.w=scrollrect->w - len;
	        from.h=scoresurface->h;
	
        	to.x=scrollrect->x + len;
	        to.y=scrollrect->y;
	        to.w=from.w;
	        to.h=from.h;
	
        	SDL_BlitSurface(scoresurface,&from,screen,&to);
	}
}
