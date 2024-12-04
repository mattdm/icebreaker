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

/* special thanks to Enrico for the grid coloring stuff for themes */

#include <SDL.h>
#include <stdio.h>

#include "icebreaker.h"
#include "cursor.h"
#include "penguin.h"
#include "line.h"
#include "grid.h"
#include "laundry.h"
#include "sound.h"
#include "globals.h"
#include "level.h"
#include "status.h"
#include "text.h"
#include "dialog.h"
#include "menu.h"
#include "options.h"
#include "themes.h"
#include "event.h"
#include "delay.h"

// fix -- make this not need to be a global
Line line1;
Line line2;

// fix -- good candidate for dynamic memory allocation
Penguin pg_flock[MAXPENGUINS];
int penguincount=0;

static int lives=0;
static int clear=0;
static long score=0;

static void setuplevel(void);

static void levelaction_startclick(int mousex, int mousey, LineType linetype);
static void levelaction_switchclick(LineType* linetypepointer);

void setuplevel()
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



LevelExitType playlevel(int level, long oldscore, ScoreSheet * levelscore)
{
	LevelExitType returncode=ERROR;
	int i,mx,my;

	SDL_Rect menubuttonrect;
	int menubuttonglow=false;
	int menubuttonpressed=false;
	int paused=false;

	int linedoneflag=false;
	LineType linetype;
	float scoremod=1;
	float bonusmod=1;

	int percentbonus=PERCENTBONUS; // right now, there's really no point in making this a variable but it doesn't hurt, and keeps things consistant
	int percentextrabonus=PERCENTEXTRABONUS-(level-1)/5; // this needs to decrease, or else the bonus disintegrates as the space required to actually trap penguins increases
					
	int tick=0;
	int timepenalty=0;
	int timepenaltyinterval=0;
	
	int domenuflag = false;
	
	int movecursorleftflag = false;
	int movecursorrightflag = false;
	int movecursorupflag = false;
	int movecursordownflag = false;
	int keyboardcounter=0;
	int keyboarddelay=0;
	
	Uint32 loopstartticks;
	
	int done = false;
	
	SDL_Event event;


	penguincount=level+1;
	lives=level+1;
	clear=0;
	score=oldscore;

	switch(options.difficulty)
	{
		case NORMAL:
			scoremod=(level+1)/2.0;
			bonusmod=(level+1)/2.0;
			timepenaltyinterval=100;
		break;
		case EASY:
			scoremod=(level+2)/6.0;
			bonusmod=(level+1)/9.0;
			timepenaltyinterval=200;
		break;
		case HARD:
			scoremod=(level+1)/1.75;
			bonusmod=(level+1)/1.5;
			timepenaltyinterval=75;
		break;
		default:
			fprintf(stderr,"Unknown difficulty -- that can't happen!\n");
		break;
	}

	levelscore->basescore=0;
	levelscore->clearbonus=0;
	levelscore->lifebonus=0;
	
	setuplevel();
	
	setcursor(CURSORVERTICAL); linetype=VERTICAL; 

/*	printf("===========================================================\n"
	       "Starting level %d.\n"
	       "Lives: %d\n",
	       level,lives);
*/	       

	menubuttonrect.x=WIDTH-(CHARWIDTH*2*4)-MARGINRIGHT-4;
	menubuttonrect.y=BOTTOMSTATUSY;
	menubuttonrect.w=CHARWIDTH*2*4+3;
	menubuttonrect.h=CHARHEIGHT*2+3;
	drawmenubutton(&menubuttonrect,false);

	line1=createline(1);
	line2=createline(2);

	for (i=0;i<penguincount;i++)
	{
		pg_flock[i] = createpenguin();
	}

	updatestatuslives(lives);
	updatestatuscleared(clear);
	updatestatusscore(score);
	updatehiscorebox();	

	clean();
	
	do 
	{
		loopstartticks = SDL_GetTicks();
	
		// FIX -- this is way too messy. time to split it up into
		// neat little functions or something. Especially the menubutton stuff.
		while (pollevent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				lives=0; // is this the right way?
				done = true;
				// fix -- should delete penguins, etc. here
				return QUIT;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				//if (grid[event.motion.x][event.motion.y] == ' ' || grid[event.motion.x][event.motion.y] == '*')
				if (event.motion.x>BORDERLEFT && event.motion.x<BORDERRIGHT && event.motion.y>BORDERTOP && event.motion.y<BORDERBOTTOM)	
				{
					switch (linetype)
					{
						case HORIZONTAL:
							setcursor(CURSORHORIZONTAL);
						break;
						case VERTICAL:
							setcursor(CURSORVERTICAL);
						break;
					}
				}			
				else  // we're somewhere outside of the playing area
				{
					setcursor(CURSORARROW);
				}
				
				if (event.motion.x>=menubuttonrect.x && event.motion.y>=menubuttonrect.y && event.motion.x<menubuttonrect.x+menubuttonrect.w && event.motion.y<menubuttonrect.y+menubuttonrect.h)
				{ // over the menu button
					if (!menubuttonglow)
					{
						menubuttonglow=true;
						drawmenubutton(&menubuttonrect,menubuttonglow);
					}
				}
				else 
				{
					if (menubuttonglow && !menubuttonpressed)
					{
						menubuttonglow=false;
						drawmenubutton(&menubuttonrect,menubuttonglow);
					}
				
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button==1) //left
				{
					// in game area?
					if (event.button.x>BORDERLEFT && event.button.x<BORDERRIGHT && event.button.y>BORDERTOP && event.button.y<BORDERBOTTOM)
					{
						levelaction_startclick(event.button.x, event.button.y, linetype);
					}
					else if (event.button.x>=menubuttonrect.x && event.button.y>=menubuttonrect.y && event.button.x<menubuttonrect.x+menubuttonrect.w && event.button.y<menubuttonrect.y+menubuttonrect.h)
					{
						menubuttonpressed=true;
					}
				}
  				else //middle or right
  				{
  					levelaction_switchclick(&linetype);
				}
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				if (menubuttonpressed && event.button.x>=menubuttonrect.x && event.button.y>=menubuttonrect.y && event.button.x<menubuttonrect.x+menubuttonrect.w && event.button.y<menubuttonrect.y+menubuttonrect.h)
				{
					menubuttonglow=true;
					drawmenubutton(&menubuttonrect,menubuttonglow);
					domenuflag=true;
				}
				else if (menubuttonglow && menubuttonpressed)
				{
					menubuttonglow=false;
					drawmenubutton(&menubuttonrect,menubuttonglow);
				}
				menubuttonpressed=false;
			}
			else if (event.type == SDL_KEYDOWN)
			{
			
				switch(translatekeyevent(&event))
				{
					case KEYMENU:
						domenuflag=true;
					break;
					case KEYPAUSE:
						// FIX -- this doesn't work.
						// paused=!paused;
					break;
					case KEYHELP: // fix -- kludgy code duplication
						setcursor(CURSORARROW);
						if (popuphelp()==POPUPQUITGAME)
						{
							lives=0; // is this the right way?
							done = true;
							return QUIT;
						}
						getmousestate(&mx,&my);
						if (mx>BORDERLEFT && mx<BORDERRIGHT && my>BORDERTOP && my<BORDERBOTTOM)	
						{
							switch (linetype)
							{
								case HORIZONTAL:
									setcursor(CURSORHORIZONTAL);
								break;
								case VERTICAL:
									setcursor(CURSORVERTICAL);
								break;
							}
						}
					break;
					case KEYSTARTLINE:
						// in game area?
						getmousestate(&mx,&my);
						if (mx>BORDERLEFT && mx<BORDERRIGHT && my>BORDERTOP && my<BORDERBOTTOM)
							levelaction_startclick(mx, my, linetype);
						else if (menubuttonglow && mx>=menubuttonrect.x && my>=menubuttonrect.y && mx<menubuttonrect.x+menubuttonrect.w && my<menubuttonrect.y+menubuttonrect.h)
							domenuflag=true;
					break;
					case KEYSWITCHLINE:
	  					levelaction_switchclick(&linetype);
					break;
					case KEYMOVELEFT:
						movecursorleftflag = true;
					break;
					case KEYMOVERIGHT:
						movecursorrightflag = true;
					break;
					case KEYMOVEUP:
						movecursorupflag = true;
					break;
					case KEYMOVEDOWN:
						movecursordownflag = true;
					break;
					case KEYMOVELEFTUP:
						movecursorleftflag = true; movecursorupflag = true;
					break;
					case KEYMOVERIGHTUP:
						movecursorrightflag = true; movecursorupflag = true;
					break;
					case KEYMOVELEFTDOWN:
						movecursorleftflag = true; movecursordownflag = true;
					break;
					case KEYMOVERIGHTDOWN:
						movecursorrightflag = true; movecursordownflag = true;
					break;
					default:
					break;
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				switch(translatekeyevent(&event))
				{
					case KEYMOVELEFT:
						movecursorleftflag = false;
					break;
					case KEYMOVERIGHT:
						movecursorrightflag = false;
					break;
					case KEYMOVEUP:
						movecursorupflag = false;
					break;
					case KEYMOVEDOWN:
						movecursordownflag = false;
					break;
					case KEYMOVELEFTUP:
						movecursorleftflag = false; movecursorupflag = false;
					break;
					case KEYMOVERIGHTUP:
						movecursorrightflag = false; movecursorupflag = false;
					break;
					case KEYMOVELEFTDOWN:
						movecursorleftflag = false; movecursordownflag = false;
					break;
					case KEYMOVERIGHTDOWN:
						movecursorrightflag = false; movecursordownflag = false;
					break;
					default:
					break;
				}
			}
			else if (event.type == SDL_ACTIVEEVENT && ((options.autopause == AUTOPAUSEON) || (event.active.state & SDL_APPACTIVE)))
			{   
				if (event.active.gain==0)
				{ // iconified / lost focus
					paused=true;
				}
				else // event.active.gain==1
				{ // restored /got focus
					paused=false;
				}
			}
			// fix -- other events to handle?
		}
		
		// move the mouse cursor in response to arrow keys
		if (!keyboarddelay)
		{
			if (movecursorleftflag || movecursorrightflag || movecursorupflag || movecursordownflag)
			{
				getmousestate(&mx,&my);
				mx=((mx/BLOCKWIDTH ) * BLOCKWIDTH ) + 4; // these numbers are kludgy -- really ought to change based on margin width
				my=((my/BLOCKHEIGHT) * BLOCKHEIGHT) + 3;
				                
				if (movecursorleftflag)
				{
					if (mx>BORDERLEFT)
						mx-=BLOCKWIDTH;
					else
						movecursorleftflag=false;
				}
				if (movecursorrightflag)
				{
					if (mx<BORDERRIGHT)
						mx+=BLOCKWIDTH;
					else
						movecursorrightflag=false;
				}
				if (movecursorupflag)
				{
					if (my>BORDERTOP)
						my-=BLOCKHEIGHT;
					else
						movecursorupflag=false;
				}
				if (movecursordownflag)
				{	
					if (my<BORDERBOTTOM)
						my+=BLOCKHEIGHT;
					else
						movecursordownflag=false;
				}
				jumpmouse(mx,my);
				keyboarddelay=KEYBOARDARROWTHROTTLER;
				if (keyboardcounter>=KEYBOARDARROWACCELPOINT*3)
					keyboarddelay=KEYBOARDARROWTHROTTLER/3;
				else if (keyboardcounter>=KEYBOARDARROWACCELPOINT)
					keyboarddelay=KEYBOARDARROWTHROTTLER/2;
				keyboardcounter++;
					
				
			}
			else
			{
				keyboardcounter=0;
			}
		}
		else
		{
			keyboarddelay--;
		}
		
		
		// move split-lines
		if (line1.on)
		{
			// kludgy crap to implement speed = 1 1/2
			linedoneflag=moveline(&line1);
			if (!linedoneflag && line1.speedslower)
			{
				line1.speedslower=false;
				linedoneflag=moveline(&line1);
			}
			else
			{
				line1.speedslower=true;
			}
				
			if (linedoneflag)
			{
				clear=(100-(countcleared()*100/(PLAYWIDTH*PLAYHEIGHT)));
				levelscore->basescore=(int)(clear*scoremod)-timepenalty;
				score=oldscore+levelscore->basescore;
				tick=0; // reset this to keep score from seeming to flicker around a lot. and to be nice. :)
				
				updatestatuscleared(clear);
				updatestatusscore(score);
				//printf("Cleared: %d%%\n",clear);				
			}

			if (line1.dead) 
			{
				playsound(SNDBREAK);
				killline(&line1);
				lives--;
				if (lives<0) lives=0;
				updatestatuslives(lives);
				//printf("Lives: %d\n",lives); 
			}
		}
		if (line2.on)
		{
			linedoneflag=moveline(&line2);
			if (!linedoneflag && line2.speedslower)
			{
				line2.speedslower=false;
				linedoneflag=moveline(&line2);
			}
			else
			{
				line2.speedslower=true;
			}

			if (linedoneflag)
			{
				clear=(100-(countcleared()*100/(PLAYWIDTH*PLAYHEIGHT)));
				levelscore->basescore=(int)(clear*scoremod)-timepenalty;
				score=oldscore+levelscore->basescore;
				tick=0; // reset this to keep score from seeming to flicker around a lot. and to be nice. :)

				updatestatuscleared(clear);
				updatestatusscore(score);
				//printf("Cleared: %d%%\n",clear);
			}

			if (line2.dead)
			{ 
				playsound(SNDBREAK);
				killline(&line2);
				lives--;
				if (lives<0) lives=0;
				updatestatuslives(lives);
				//printf("Lives: %d\n",lives);
				
			}
		}
		
	
		// move (and get old background)
		for (i=0;i<penguincount;i++)
		{
			
			soil(pg_flock[i].geom); // mark the penguin's old position as dirty
			movepenguin(&pg_flock[i]);
		
			soil(pg_flock[i].geom); // mark the penguin's new position as dirty too (it will be soon...)
			savebehindpenguin(&pg_flock[i]);
		}
		

		// actually draw
		for (i=0;i<penguincount;i++)
		{
			drawpenguin(&pg_flock[i]);
		}

		if (domenuflag)
		{
			setcursor(CURSORARROW);
			switch (popuplevelmenu())
			{
				case POPUPQUITGAME:
					lives=0; // is this the right way?
					done = true;
					return QUIT;
				break;
				case POPUPNEWGAME:
					// hmmm... maybe this could be done better
					done = true;
					if (checkiflevelatstart())
						returncode=ZERO;
					else
						returncode=DEAD;
				break;
				default:
					// if we don't recognize the code, do nothing.
				break;
			}
			
			domenuflag=false;
		}

		
		// update clock
		tick++;
		if (tick>timepenaltyinterval)
		{
			tick=0;
			if (levelscore->basescore>0)
			{
				timepenalty++;
				levelscore->basescore--;
				score=oldscore+levelscore->basescore;
				updatestatusscore(score);
			}
		}
		
		// update screen
		clean();

		if (paused) 
		{
			SDL_WaitEvent(NULL);
		}


		// clear for next update
		for (i=0;i<penguincount;i++)
		{
			erasepenguin(&pg_flock[i]);
		}
		
		if (lives<=0) // game over
		{
			done = true;
			returncode=DEAD;
		} 
		else if (!line1.on && !line2.on && clear>=PERCENTREQUIRED) // success!
		{ 
			done = true;
			levelscore->basescore=(int)(clear*scoremod)-timepenalty;
			returncode=PASS;


			levelscore->clearbonus=0;
			// bonuses for clearing lots
			if (clear>percentbonus)
				levelscore->clearbonus+=(int)((clear-percentbonus)*bonusmod);
			if (clear>percentextrabonus)
				levelscore->clearbonus+=(int)((clear-percentextrabonus)*(clear-percentextrabonus)*bonusmod);
				
			// bonuses for leftover lives
			levelscore->lifebonus=(int)((lives-1)*3*bonusmod);
		} 

		//printboard();
		
		delaytil(loopstartticks+MSECPERFRAME);

		
	} while (!done);

	// make sure visible penguins are actually in the screen memory,
	// and then erase them so we don't leak surfaces.
	for (i=0;i<penguincount;i++)
	{
		drawpenguin(&pg_flock[i]);
		deletepenguin(&pg_flock[i]);
	}

	clean();
	return returncode;
}



// this function at least makes us able to keep the level variables local
// to this file instead of totally global...
void redrawwholelevel()
{
	int i;
	SDL_FillRect(screen,NULL,color.background);
	drawgridblocks();

	updatehiscorebox();
	updatestatusscore(score);
	updatestatuslives(lives);
	updatestatuscleared(clear);
	
	// FIX -- changing theme makes line appear to grow
	// by one pixel. that's kinda gross -- we should correct that.
	if (line1.on) SDL_FillRect(screen,&(line1.geom),*(line1.colorpointer));
	if (line2.on) SDL_FillRect(screen,&(line2.geom),*(line2.colorpointer));
	
	for (i=0;i<penguincount;i++)
	{
		resetpenguinimage(&pg_flock[i]);
		savebehindpenguin(&pg_flock[i]);
		drawpenguin(&pg_flock[i]);
	}

}

int checkiflevelatstart(void)
{
	return (clear==0 && score==0);
}

void levelaction_startclick(int mousex, int mousey, LineType linetype)
{
	int x,y,xdif,ydif;

	x=(((mousex-BORDERLEFT)/BLOCKWIDTH ) * BLOCKWIDTH ) +BORDERLEFT;
	y=(((mousey-BORDERTOP)/BLOCKHEIGHT) * BLOCKHEIGHT) +BORDERTOP;
	xdif=mousex-x; ydif=mousey-y;

	if (grid[x][y] == '*' || grid[mousex][mousey] == '*') 
	{ // a little bit of grace if the player clicks directly on the penguin
		playsound(SNDOUCH);
	}
	else if (grid[x][y] == ' ')
	{
		switch (linetype)
		{
			case VERTICAL:
				if (!line1.on)
					(ydif<BLOCKHEIGHT/2) ? startline(&line1,UP,x,y) : startline(&line1,UP,x,y+BLOCKHEIGHT);
				if (!line2.on)
					(ydif<BLOCKHEIGHT/2) ? startline(&line2,DOWN,x,y) : startline(&line2,DOWN,x,y+BLOCKHEIGHT);
			break;
			case HORIZONTAL:
				if (!line1.on) 
					(xdif<BLOCKWIDTH/2) ? startline(&line1,LEFT,x,y) : startline(&line1,LEFT,x+BLOCKWIDTH,y);
				if (!line2.on)
					(xdif<BLOCKWIDTH/2) ? startline(&line2,RIGHT,x,y) : startline(&line2,RIGHT,x+BLOCKWIDTH,y);
			break;
		}
	}
}

void levelaction_switchclick(LineType* linetypepointer)
{
	switch (*linetypepointer)
	{
		case VERTICAL:
			*linetypepointer=HORIZONTAL;
			setcursor(CURSORHORIZONTAL); 
		break;
		case HORIZONTAL:
			*linetypepointer=VERTICAL;
			setcursor(CURSORVERTICAL);
		break;
	}
}
