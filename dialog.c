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

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include "icebreaker.h"
#include "globals.h"
#include "text.h"
#include "laundry.h"
#include "dialog.h"
#include "sound.h"
#include "hiscore.h"
#include "options.h"
#include "fullscreen.h"
#include "themes.h"
#include "grid.h"
#include "status.h"
#include "level.h"
#include "event.h"


PopupReturnType gethighusername(int highest)
{
	SDL_Rect tmprect,namerect,okayrect;
	SDL_Event event;
	int done=false;
	PopupReturnType rc=POPUPDONOTHING;
	int okayglow=false;
	int okaypressed=false;
	int clearname=true;
	
	char keypressed;
	int insertionpoint=0;
	
	tmprect.w=282;
	tmprect.h=92;
	tmprect.x=(WIDTH/2)-(tmprect.w/2)-1;
	tmprect.y=(HEIGHT/2)+31;
	SDL_FillRect(screen,&tmprect,color.gridline);

	tmprect.w-=2; tmprect.h-=2;
	tmprect.x++; tmprect.y++;
	SDL_FillRect(screen,&tmprect,color.background);

	if (highest)
		puttext(tmprect.x+7,tmprect.y+7,2,color.normaltext,"NEW TOP SCORE!");
	else
		puttext(tmprect.x+7,tmprect.y+7,2,color.normaltext,"NEW HIGH SCORE.");
	puttext(tmprect.x+7,tmprect.y+7+16,2,color.normaltext,"ENTER YOUR NAME:");
	
	namerect.w=268;
	namerect.h=29;
	namerect.x=tmprect.x+6;
	namerect.y=tmprect.y+7+32;
	
	SDL_FillRect(screen,&namerect,color.textentrybox);

	// this makes sure username fits. maybe a bit gross to have this
	// truncated here, but hey.
	while(strlen(username)>1 && gettextwidth(4,username) > namerect.w-8)
	{
		username[strlen(username)-1]='\0';
	}

	puttext(namerect.x+4,namerect.y+5,4,color.textentrytext,username);

	okayrect.x=tmprect.x+tmprect.w-32;
	okayrect.y=tmprect.y+7+64;
	okayrect.h=CHARHEIGHT*2+3;
	okayrect.w=CHARWIDTH*2*2+1;

	//SDL_FillRect(screen,&okayrect,color.textentrybox);
	puttext(okayrect.x+3,okayrect.y+3,2,color.normaltext,"OK");


	updateall();  // fix -- is this really needed? we should soil() the
	              // above rectangles instead, i  think


	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(500,90);
	
	clean();
	do
	{
		SDL_WaitEvent(NULL); // no new CPU cooler needed. :)
		while (pollevent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				done=true;
				rc=POPUPQUITGAME;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{  // fix -- make this left button only
				if (event.button.x>=okayrect.x
				 && event.button.y>=okayrect.y
				 && event.button.x<okayrect.x+okayrect.w
				 && event.button.y<okayrect.y+okayrect.h)
				okaypressed=true;
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				if (okaypressed
				 && event.button.x>=okayrect.x
				 && event.button.y>=okayrect.y
				 && event.button.x<okayrect.x+okayrect.w
				 && event.button.y<okayrect.y+okayrect.h)
				{
					if (username[0]!='\0')
					{
						done=true;
						rc=POPUPOK;
					}
					else
					{
						playsound(SNDOUCH);
					}
				}
				else if (okayglow)
				{
					okayglow=false;
					SDL_FillRect(screen,&okayrect,color.background);
					puttext(okayrect.x+3,okayrect.y+3,2,color.normaltext,"OK");
					soil(okayrect);
				}
				okaypressed=false;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				if (event.motion.x>=okayrect.x
				 && event.motion.y>=okayrect.y
				 && event.motion.x<okayrect.x+okayrect.w
				 && event.motion.y<okayrect.y+okayrect.h)
				 {
					if (!okayglow)
					{
						okayglow=true;
						SDL_FillRect(screen,&okayrect,color.menuhighlight);
						puttext(okayrect.x+3,okayrect.y+3,2,color.background,"OK");
						soil(okayrect);
					}
					
				}
				else
				{
					if (okayglow && !okaypressed)
					{
						okayglow=false;
						SDL_FillRect(screen,&okayrect,color.background);
						puttext(okayrect.x+3,okayrect.y+3,2,color.normaltext,"OK");
						soil(okayrect);
					}
				}
			
			}
			else if (event.type==SDL_KEYDOWN)
			{

				if (event.key.keysym.sym==SDLK_BACKSPACE ||
				    event.key.keysym.sym==SDLK_DELETE ||
				    event.key.keysym.sym==SDLK_LEFT)
				{
					if (clearname)
					{
						insertionpoint=0;
						username[0]='\0'; // null out username
					}
					else if (insertionpoint>0)
					{
						insertionpoint--;	
						username[insertionpoint]='\0';
					}
					else
					{
						playsound(SNDOUCH);
					}
					clearname=false;
				}	
				else if (event.key.keysym.sym==SDLK_RETURN || event.key.keysym.sym==SDLK_KP_ENTER)
				{
					if (username[0]!='\0')
					{
						done=true;
						rc=POPUPOK;
					}
					else
					{
						playsound(SNDOUCH);
					}
					clearname=false;
				}
				else if (event.key.keysym.sym==SDLK_ESCAPE || event.key.keysym.sym==SDLK_CLEAR) // fix -- make ctrl-u work here
				{
					insertionpoint=0;
					username[0]='\0'; // null out username
					clearname=false;
				}
				else if ((event.key.keysym.unicode & 0xFF80) == 0) // make sure it's ascii
				{
					keypressed=event.key.keysym.unicode & 0x7F;
					if (keypressed==32) keypressed=95;
					if (keypressed>32 && keypressed<127)
					{
						if (clearname)
						{
							insertionpoint=0;
							clearname=false;
						}
						if (insertionpoint<(50-1) && gettextwidth(4,username)+getletterwidth(4,keypressed) <= namerect.w-8) //(insertionpoint<12)
						{
							username[insertionpoint]=keypressed;
							username[insertionpoint+1]='\0';
							insertionpoint++;
						}
						else
						{
							playsound(SNDOUCH);
						}
					}			
				}
				SDL_FillRect(screen,&namerect,color.textentrybox);
				puttext(namerect.x+4,namerect.y+5,4,color.textentrytext,username);
				soil(namerect);	
			}
		}
		clean();
	} while (!done);

	// because the game is over, restore of background is unnecessary
	
	SDL_EnableKeyRepeat(0,0);
	SDL_EnableUNICODE(0);
	return(rc);
}


PopupReturnType popuphighscores()
{
	SDL_Rect scorelistrect,borderrect;
	SDL_Surface * scorelistsave;
	PopupReturnType rc=POPUPDONOTHING;
	int i;
	char buf[30]; // plenty big. :)

	scorelistrect.w=PLAYWIDTH-(BLOCKWIDTH*11);
	scorelistrect.h=PLAYHEIGHT-(BLOCKHEIGHT*4);
	scorelistrect.x=BORDERLEFT+PLAYWIDTH/2-scorelistrect.w/2+BLOCKWIDTH*4;
	scorelistrect.y=BORDERTOP+PLAYHEIGHT/2-scorelistrect.h/2+BLOCKHEIGHT/2;
	
	borderrect.w=scorelistrect.w+2;
	borderrect.h=scorelistrect.h+2;
	borderrect.x=scorelistrect.x-1;
	borderrect.y=scorelistrect.y-1;

	scorelistsave = SDL_CreateRGBSurface(SDL_SWSURFACE,borderrect.w,borderrect.h,screen->format->BitsPerPixel,0,0,0,0);
	SDL_BlitSurface(screen, &borderrect, scorelistsave, NULL);

	SDL_FillRect(screen,&borderrect,color.gridline);
	SDL_FillRect(screen,&scorelistrect,color.background);

	puttext(scorelistrect.x+(scorelistrect.w/2-(CHARWIDTH*2*9)),scorelistrect.y+BLOCKHEIGHT,4,color.bonusscrolltext,"HIGH SCORES");

	// just in case they've changed...
	readhiscores();

	for (i=0;i<HISCORENUM;i++)
	{
		snprintf(buf,4,"%d.",i+1);
		puttext(scorelistrect.x+BLOCKWIDTH,scorelistrect.y+45+i*(CHARHEIGHT*2+5),2,color.normaltext,buf);		
		puttext(scorelistrect.x+BLOCKWIDTH*4,scorelistrect.y+45+i*(CHARHEIGHT*2+5),2,color.normaltext,hiscorename[i]);		
		snprintf(buf,30,"%ld",hiscoreval[i]);		
		puttext(scorelistrect.x+scorelistrect.w-(BLOCKWIDTH*5),scorelistrect.y+45+i*(CHARHEIGHT*2+5),2,color.normaltext,buf);
	}
	
	soil(borderrect);	

	clean();
		
	rc=waitforuser();

	SDL_BlitSurface(scorelistsave, NULL, screen, &borderrect);
	soil(borderrect);
	clean();

	SDL_FreeSurface(scorelistsave);	 	

	return(rc);
}

PopupReturnType popuphelp()
{
	SDL_Rect helprect,borderrect;
	SDL_Surface * helpsave;
	PopupReturnType rc=POPUPDONOTHING;
	int i=0;
	char buf[80];


#ifdef ONEBUTTONMICEARESILLY
	char helptext[13][50]={
	     "Ok, so there's a bunch of penguins on an",
	     "iceberg in Antarctica. You must catch them",
	     "so they can be shipped to Finland. The",
	     "smaller the area in which they're caught,",
	     "the lower the shipping fees and the higher",
	     "your score - but don't take too long: the",
	     "clock is ticking. Once 80% of the 'berg is",
	     "cleared, it's on to the next shipment.",
	     "",
	     "The left mouse button starts lines; right",
	     "button or [spacebar] toggles direction.",
	     "",
	     "Check the README file for more info."
	      };
#else
	char helptext[13][50]={
	     "Ok, so there's a bunch of penguins on an",
	     "iceberg in Antarctica. You must catch them",
	     "so they can be shipped to Finland. The",
	     "smaller the area in which they're caught,",
	     "the lower the shipping fees and the higher",
	     "your score - but don't take too long: the",
	     "clock is ticking. Once 80% of the 'berg is",
	     "cleared, it's on to the next shipment.",
	     "",
	     "The left mouse button starts lines; right",
	     "button toggles direction. You'll catch on.",
	     "",
	     "Check the README file for more info."
	      };
#endif

	helprect.w=PLAYWIDTH-(BLOCKWIDTH*2)+2;
	helprect.h=PLAYHEIGHT-(BLOCKHEIGHT*3);
	helprect.x=BORDERLEFT+PLAYWIDTH/2-helprect.w/2;
	helprect.y=BORDERTOP+PLAYHEIGHT/2-helprect.h/2;
	
	borderrect.w=helprect.w+2;
	borderrect.h=helprect.h+2;
	borderrect.x=helprect.x-1;
	borderrect.y=helprect.y-1;

	helpsave = SDL_CreateRGBSurface(SDL_SWSURFACE,borderrect.w,borderrect.h,screen->format->BitsPerPixel,0,0,0,0);
	SDL_BlitSurface(screen, &borderrect, helpsave, NULL);

	SDL_FillRect(screen,&borderrect,color.gridline);
	SDL_FillRect(screen,&helprect,color.background);
	
	for (i=0;i<13;i++)
	{
		puttext(helprect.x+BLOCKWIDTH/2,helprect.y+BLOCKHEIGHT/2+i*(CHARHEIGHT*2+4),2,color.normaltext,helptext[i]);		
	}
	snprintf(buf,80,"v%d.%d.%d   %s",VERMAJOR,VERMINOR,VERSUB,"Copyright (c) 2000-2002 Matthew Miller. Released under the GPL.");
	puttext(helprect.x+BLOCKWIDTH/2,helprect.y+helprect.h-CHARHEIGHT*3,1,color.copyrighttext,buf);		
	puttext(helprect.x+BLOCKWIDTH/2,helprect.y+helprect.h-CHARHEIGHT*1-3,1,color.copyrighttext,"Thanks to my wonderful wife Karen for inspiration (and for patience)!");		
	
	soil(borderrect);	

	clean();

	rc=waitforuser();

	SDL_BlitSurface(helpsave, NULL, screen, &borderrect);
	soil(borderrect);
	clean();

	SDL_FreeSurface(helpsave);	 	

	return(rc);
}


/* Wait for the user to hit a key or button or quit; returns true if quit */
PopupReturnType waitforuser()
{
	int done=false;
	PopupReturnType rc=POPUPDONOTHING;
	SDL_Event event;
	
	do
	{
		SDL_WaitEvent(NULL);
		while(pollevent(&event))
		{
			if ( event.type == SDL_QUIT)
			{
				done=true;
				rc=POPUPQUITGAME;
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				done=true;
			}
			else if (event.type == SDL_KEYUP)
			{
				switch(translatekeyevent(&event))
				{
					case KEYCANCEL:     // falls through
					case KEYMENU:       // falls through
					case KEYSWITCHLINE: // falls through
					case KEYSTARTLINE:  
						done=true;
					default:
					break;
				}
	
			}
		}
	} while (!done);
	return(rc);		
}


// fix -- make this more generic (one or two buttons, selectable button text)
PopupReturnType yesnodialog(char* text1, char* text2, char* text3)
{
	SDL_Rect tmprect, yesrect,norect;
	SDL_Event event;
	int done=false;
	int rc=POPUPDONOTHING;
	int yesglow=false;
	int yespressed=false;
	int noglow=false;
	int nopressed=false;
	SDL_Surface* bgsave;
	
	tmprect.w=gettextwidth(2,text1);
	if (gettextwidth(2,text2)>tmprect.w) tmprect.w=gettextwidth(2,text2);
	if (gettextwidth(2,text3)>tmprect.w) tmprect.w=gettextwidth(2,text3);
	tmprect.w+=15;
	tmprect.h=92; // fix -- make dialog shorter if text3=null. maybe also text2, but that might be too short
	tmprect.x=(WIDTH/2)-(tmprect.w/2);
	tmprect.y=(HEIGHT/2)-(tmprect.h/2);
	
	// fix -- error checking if this fails
	bgsave = SDL_CreateRGBSurface(SDL_SWSURFACE,tmprect.w,tmprect.h,screen->format->BitsPerPixel,0,0,0,0);
	SDL_BlitSurface(screen, &tmprect, bgsave, NULL);

	SDL_FillRect(screen,&tmprect,color.gridline);
	
	tmprect.w-=2; tmprect.h-=2;
	tmprect.x++; tmprect.y++;
	SDL_FillRect(screen,&tmprect,color.background);
	tmprect.w+=2; tmprect.h+=2; // yay kludgy
	tmprect.x--; tmprect.y--;
	
	

	puttext(tmprect.x+(tmprect.w-gettextwidth(2,text1))/2,tmprect.y+7,2,color.normaltext,text1);
	puttext(tmprect.x+(tmprect.w-gettextwidth(2,text2))/2,tmprect.y+7+CHARHEIGHT*3,2,color.normaltext,text2);
	puttext(tmprect.x+(tmprect.w-gettextwidth(2,text3))/2,tmprect.y+7+CHARHEIGHT*6,2,color.normaltext,text3);
	
	norect.x=tmprect.x+tmprect.w-33;
	norect.y=tmprect.y+7+64;
	norect.h=CHARHEIGHT*2+3;
	norect.w=CHARWIDTH*2*2+1;
	
	yesrect.x=tmprect.x+7;
	yesrect.y=tmprect.y+7+64;
	yesrect.h=CHARHEIGHT*2+3;
	yesrect.w=CHARWIDTH*2*3+1;

	puttext(norect.x+3,norect.y+3,2,color.normaltext,"NO");
	puttext(yesrect.x+3,yesrect.y+3,2,color.normaltext,"YES");

	
	soil(tmprect);
	
	clean();
	do
	{
		SDL_WaitEvent(NULL);
		while (pollevent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				done=true;
				rc=POPUPQUITGAME;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{  // fix -- make this left button only
				if (event.button.x>=norect.x
				 && event.button.y>=norect.y
				 && event.button.x<norect.x+norect.w
				 && event.button.y<norect.y+norect.h)
				nopressed=true;
				else if (event.button.x>=yesrect.x
				 && event.button.y>=yesrect.y
				 && event.button.x<yesrect.x+yesrect.w
				 && event.button.y<yesrect.y+yesrect.h)
				yespressed=true;
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				if (nopressed
				 && event.button.x>=norect.x
				 && event.button.y>=norect.y
				 && event.button.x<norect.x+norect.w
				 && event.button.y<norect.y+norect.h)
				{
					done=true;
					rc=POPUPNO;
				}
				else if (noglow)
				{
					noglow=false;
					SDL_FillRect(screen,&norect,color.background);
					puttext(norect.x+3,norect.y+3,2,color.normaltext,"NO");
					soil(norect);
				}
				nopressed=false;
				
				if (yespressed
				 && event.button.x>=yesrect.x
				 && event.button.y>=yesrect.y
				 && event.button.x<yesrect.x+yesrect.w
				 && event.button.y<yesrect.y+yesrect.h)
				{
					done=true;
					rc=POPUPYES;
				}
				else if (yesglow)
				{
					yesglow=false;
					SDL_FillRect(screen,&yesrect,color.background);
					puttext(yesrect.x+3,yesrect.y+3,2,color.normaltext,"YES");
					soil(yesrect);
				}
				yespressed=false;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				if (event.motion.x>=norect.x
				 && event.motion.y>=norect.y
				 && event.motion.x<norect.x+norect.w
				 && event.motion.y<norect.y+norect.h)
				 {
					if (!noglow)
					{
						noglow=true;
						SDL_FillRect(screen,&norect,color.menuhighlight);
						puttext(norect.x+3,norect.y+3,2,color.background,"NO");
						soil(norect);
					}
					
				}
				else
				{
					if (noglow && !nopressed)
					{
						noglow=false;
						SDL_FillRect(screen,&norect,color.background);
						puttext(norect.x+3,norect.y+3,2,color.normaltext,"NO");
						soil(norect);
					}
				}
				if (event.motion.x>=yesrect.x
				 && event.motion.y>=yesrect.y
				 && event.motion.x<yesrect.x+yesrect.w
				 && event.motion.y<yesrect.y+yesrect.h)
				 {
					if (!yesglow)
					{
						yesglow=true;
						SDL_FillRect(screen,&yesrect,color.menuhighlight);
						puttext(yesrect.x+3,yesrect.y+3,2,color.background,"YES");
						soil(yesrect);
					}
					
				}
				else
				{
					if (yesglow && !yespressed)
					{
						yesglow=false;
						SDL_FillRect(screen,&yesrect,color.background);
						puttext(yesrect.x+3,yesrect.y+3,2,color.normaltext,"YES");
						soil(yesrect);
					}
				}
			
			}
			else if (event.type==SDL_KEYDOWN)
			{
				if (event.key.keysym.sym==SDLK_n)
				{
					done=true;
					rc=POPUPNO;
				}	
				else if (event.key.keysym.sym==SDLK_ESCAPE)
				{
					// little hack to keep escape key from "bleeding" into
					// cancelling the whole menu
					done=true;
					rc=POPUPYES;
					SDL_Delay(200);
					while(pollevent(&event) && (event.type==SDL_KEYDOWN || event.type==SDL_KEYUP));
				}
				else if (event.key.keysym.sym==SDLK_y || event.key.keysym.sym==SDLK_RETURN || event.key.keysym.sym==SDLK_KP_ENTER)
				{
					done=true;
					rc=POPUPYES;
				}
			}
		}
		clean();
	} while (!done);

	SDL_BlitSurface(bgsave, NULL, screen, &tmprect);
	soil(tmprect);
	clean();
	        
	return(rc);
}
