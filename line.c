/*
* IceBreaker
* Copyright (c) 2000-2002 Matthew Miller <mattdm@mattdm.org>
*   http://www.mattdm.org/
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
#include "icebreaker.h"
#include "globals.h"
#include "line.h"
#include "laundry.h"
#include "grid.h"
#include "themes.h"

Line createline(int linenum)
{
	Line l;

	switch (linenum)
	{
		case 1: 
			l.id='1';
			l.colorpointer=&(color.line1);
		break;
		case 2:
			l.id='2'; 
			l.colorpointer=&(color.line2);
		break;
		default: 
			l.id='L';
			l.colorpointer=&(color.background);
		break;
	}
		
	l.on=false;
	l.dir=UP;
	l.geom.x=0;
	l.geom.y=0;
	l.geom.w=BLOCKWIDTH;
	l.geom.h=BLOCKHEIGHT;
	l.mark=l.geom;
	l.dead=false;
	l.speedslower=false;
	l.stuckcount=0;
	return(l);
}

void startline(Line * l, LineDir d, int x, int y)
{
	l->on=true;
	l->dir=d;
	l->stuckcount=0;

	switch (d)
	{
		case UP: 
			l->geom.w=BLOCKWIDTH;
			l->geom.h=1;
			l->geom.x=x;
			l->geom.y=y-1; 
		break;
		case DOWN:
			l->geom.w=BLOCKWIDTH;
			l->geom.h=1;
			l->geom.x=x;
			l->geom.y=y; 
		break;
		case LEFT: 
			l->geom.w=1;
			l->geom.h=BLOCKHEIGHT;
			l->geom.x=x-1;
			l->geom.y=y;			
		break;
		case RIGHT:
			l->geom.w=1;
			l->geom.h=BLOCKHEIGHT;
			l->geom.x=x;
			l->geom.y=y;
		break;
	}
	l->mark=l->geom;
}


int moveline(Line * l)
{
	int finish=false;
	char check1;
	char check2;

	markgrid(l->mark.x,l->mark.y,l->mark.w,l->mark.h,l->id);
	SDL_FillRect(screen,&(l->mark),*(l->colorpointer));
	soil(l->mark);

	switch (l->dir)
	{
		case UP:
			check1=grid[l->geom.x][l->geom.y-1];
			check2=grid[l->geom.x+BLOCKWIDTH-1][l->geom.y-1];
		break;
		case DOWN:
			check1=grid[l->geom.x][l->geom.y+l->geom.h];
			check2=grid[l->geom.x+BLOCKWIDTH-1][l->geom.y+l->geom.h];
		break;
		case LEFT:
			check1=grid[l->geom.x-1][l->geom.y];
			check2=grid[l->geom.x-1][l->geom.y+BLOCKHEIGHT-1];
		break;
		case RIGHT:
			check1=grid[l->geom.x+l->geom.w][l->geom.y];
			check2=grid[l->geom.x+l->geom.w][l->geom.y+BLOCKHEIGHT-1];
		break;
		default: // this will never happen. really.
			fprintf(stderr,"Line has no direction. That shouldn't have happened.\n");
			check1='!';
			check2='!';
		break;
	}
	

	if (check1 == ' ' && check2 == ' ')
	{ // next space is empty
		switch (l->dir)
		{
			case UP:
				l->geom.y--;
				l->geom.h++;
				l->mark.y--;
				l->mark.h=1;
			break;
			case DOWN:
				l->geom.h++; // increase length of line -- top stays same 
				l->mark.y+=l->mark.h;
				l->mark.h=1;
			break;
			case LEFT:
				l->geom.x--;
				l->geom.w++;
				l->mark.x--;
				l->mark.w=1;
			break;
			case RIGHT:
				l->geom.w++; // increase width of line -- left side stays same 
				l->mark.x+=l->mark.w;
				l->mark.w=1;
			break;
		}
	}
	else if (check1 == '*' || check2 == '*')
	{ // hit a penguin. kills line.
		l->dead=true;
	}
	else if (check1 == '1' || check2 == '1' || check1 == '2' || check2 == '2')
	{
		if (l->stuckcount>LINEMAXSTUCK)
		{
			// FIX -- should play sound when this happens, to
			// let users know that this isn't a mistake.
			finish=true;
		}
		else
		{
			l->stuckcount++;
			
			// FIX: kludge-o-rama!!
			// this could work around the irritating thing where
			// a line gets started 'on top' of another line. but it
			// is totally repairing the symptom, not the bug. *sigh*
			//if (l->geom.w==1 || l->geom.h==1) finish=true;
		}
	}
	else
	{ // hit something else
		finish=true;
	}
	
	if (finish)
	{
		markgrid(l->mark.x,l->mark.y,l->mark.w,l->mark.h,l->id);
		SDL_FillRect(screen,&(l->mark),*(l->colorpointer));
		soil(l->mark);
		finishline(l);
		return(1);
	}
	
	return(0);
}

void finishline(Line * l)
{
	int i;
	int quick1=false;
	int quick2=false;
	
	l->on=false;	
	
	//printwholegrid();
	
	switch (l->dir)
  	{
  		case DOWN:
  		// falls through.
  		case UP:
  			markgrid(l->geom.x,l->geom.y,l->geom.w,l->geom.h,'|');
  			SDL_FillRect(screen,&l->geom,color.background);
  			soil(l->geom);		
  				
  			// scan along edges to quickly determine if this
  			// is going to be complicated.
  			quick1=true; quick2=true;
  			for (i=l->geom.y+BLOCKHEIGHT/2;i<l->geom.y+l->geom.h;i++)
  			{
  				if (grid[l->geom.x-1][i] != ' ' && grid[l->geom.x-1][i] != '*') quick1=false;
  				if (grid[l->geom.x+BLOCKWIDTH][i] != ' ' && grid[l->geom.x+BLOCKWIDTH][i] != '*') quick2=false;
  			}
  				
  			//printf("Quick %d %d\n",quick1,quick2);
  				
  			checkempty(l->geom.x-BLOCKWIDTH/2,l->geom.y+BLOCKHEIGHT/2);
  			checkempty(l->geom.x+BLOCKWIDTH+BLOCKWIDTH/2/2,l->geom.y+BLOCKHEIGHT/2);
  		
  			if (!quick1)
  				for (i=l->geom.y+BLOCKHEIGHT/2+BLOCKHEIGHT;i<l->geom.y+l->geom.h;i+=BLOCKHEIGHT)
  					checkempty(l->geom.x-BLOCKWIDTH/2,i);
  			if (!quick2)
  				for (i=l->geom.y+BLOCKHEIGHT/2+BLOCKHEIGHT;i<l->geom.y+l->geom.h;i+=BLOCKHEIGHT)
  					checkempty(l->geom.x+BLOCKWIDTH+BLOCKWIDTH/2,i);
  		break;
  		case RIGHT:
  		// falls through
  		case LEFT:
  			markgrid(l->geom.x,l->geom.y,l->geom.w,l->geom.h,'-');
  			SDL_FillRect(screen,&l->geom,color.background);
  			soil(l->geom);		
	
  			// scan along edges to quickly determine if this
  			// is going to be complicated.
  			quick1=true; quick2=true;
  			for (i=l->geom.x+BLOCKWIDTH/2;i<l->geom.x+l->geom.w;i++)
  			{
  				if (grid[i][l->geom.y-1] != ' ' && grid[i][l->geom.y-1] != '*') quick1=false;
  				if (grid[i][l->geom.y+BLOCKHEIGHT] != ' ' && grid[i][l->geom.y+BLOCKHEIGHT] != '*') quick2=false;
  			}
  				
  			//printf("Quick %d %d\n",quick1,quick2);
	  			
	  		checkempty(l->geom.x+BLOCKWIDTH/2,l->geom.y-BLOCKHEIGHT/2);
	  		checkempty(l->geom.x+BLOCKWIDTH/2,l->geom.y+BLOCKHEIGHT+BLOCKHEIGHT/2);
	  			
	  		if (!quick1)
	  			for (i=l->geom.x+BLOCKWIDTH/2+BLOCKWIDTH;i<l->geom.x+l->geom.w;i+=BLOCKWIDTH)
		  			checkempty(i,l->geom.y-BLOCKHEIGHT/2);
	  			
	  		if (!quick2)
	  			for (i=l->geom.x+BLOCKWIDTH/2+BLOCKWIDTH;i<l->geom.x+l->geom.w;i+=BLOCKWIDTH)
		  			checkempty(i,l->geom.y+BLOCKHEIGHT+BLOCKHEIGHT/2);
	  		
		break;
	}
	
}


void killline(Line * l)
{
	l->on=false;
	l->dead=false;
	
	switch (l->dir)
  	{
  		case DOWN:
		// falls through.
  		case UP:
  			markgrid(l->geom.x,l->geom.y,l->geom.w,l->geom.h,' ');
  			// FIXORAMA
  			SDL_BlitSurface(gridsave,&l->geom, screen, &l->geom);
  			soil(l->geom);		
  		break;
  		case RIGHT:
  		// falls through
  		case LEFT:
  			markgrid(l->geom.x,l->geom.y,l->geom.w,l->geom.h,' ');
  			// FIXORAMA
  			SDL_BlitSurface(gridsave,&l->geom, screen, &l->geom);
  			soil(l->geom);		
	  	break;
	}
}
