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
#include "penguin.h"
#include "grid.h"
#include "line.h"
#include "level.h"
#include "options.h"
#include "themes.h"

static int createpenguinbgsave(Penguin* p);

Penguin createpenguin()
{
	return(createpenguinxy(BORDERLEFT + (random() % (PLAYWIDTH-BLOCKWIDTH)),BORDERTOP + (random() % (PLAYHEIGHT-BLOCKWIDTH))));
}

Penguin createpenguinxy(int x, int y)
{
	Penguin p;

	switch (random() % 4)
	{
		case 0:
			p.xdelta= PENGUINSPEED;p.ydelta= PENGUINSPEED;
			p.image = spritemirrorimage;
		break;
		case 1:
			p.xdelta= PENGUINSPEED;p.ydelta=-PENGUINSPEED;
			p.image = spritemirrorimage;
		break;
		case 2:
			p.xdelta=-PENGUINSPEED;p.ydelta= PENGUINSPEED;
			p.image = spriteimage;
		break;
		case 3:
			p.xdelta=-PENGUINSPEED;p.ydelta=-PENGUINSPEED;
			p.image = spriteimage;
		break;
		default: // this won't happen, but it shuts up the warning.
			p.xdelta=-PENGUINSPEED;p.ydelta=-PENGUINSPEED;
			p.image = spriteimage;
		break;
	}
	
	// For debugging only, of course -- this makes crippled penguins.
	//p.xdelta=0; p.ydelta=0;
	
	p.speedslower=0;
			
	p.geom.w=p.image->w;
	p.geom.h=p.image->h;	
			
	//p.geom.x=BORDERLEFT + (PLAYWIDTH +BLOCKWIDTH )/2;
	//p.geom.y=BORDERTOP  + (PLAYHEIGHT+BLOCKHEIGHT)/2;
			
	p.geom.x=x;
	p.geom.y=y;

	createpenguinbgsave(&p);

	return(p);
}

void deletepenguin(Penguin* p)
{
	SDL_FreeSurface(p->bgsave);
}

void resetpenguinimage(Penguin* p)
{
	SDL_FreeSurface(p->bgsave);	
	
	if (p->xdelta>0)
		p->image=spritemirrorimage;
	else
		p->image=spriteimage;
	
	p->geom.w=p->image->w;
        p->geom.h=p->image->h;
        
        
	createpenguinbgsave(p);
}


void savebehindpenguin(Penguin* p)
{
	SDL_BlitSurface(screen, &(p->geom), p->bgsave, NULL);
}	

void drawpenguin(Penguin* p)
{
	SDL_BlitSurface(p->image, NULL, screen, &(p->geom));
}

void erasepenguin(Penguin* p)
{
	SDL_BlitSurface(p->bgsave, NULL, screen, &(p->geom));
}

void movepenguin(Penguin* p)
{
	int newx, newy;
	int checkx,checky;
	int movex=0,movey=0;

	switch (options.difficulty)
	{
		case NORMAL:
			if (p->speedslower)
				{ movex=p->xdelta/2; movey=p->ydelta/2; }
			else
				{ movex=p->xdelta; movey=p->ydelta; }
			p->speedslower=!p->speedslower;
		break;
		case HARD:
			movex=p->xdelta; movey=p->ydelta;
		break;
		case EASY:
			movex=p->xdelta/2; movey=p->ydelta/2;
		break;
	}
		
	newx=p->geom.x+movex;
	newy=p->geom.y+movey;

	
	markgrid(p->geom.x,p->geom.y,BLOCKWIDTH,BLOCKHEIGHT,' ');
	
	if (movex>0) checkx = newx+BLOCKWIDTH;
	else checkx = newx;

	if (grid[checkx][p->geom.y]==' ' && grid[checkx][p->geom.y+BLOCKHEIGHT-1]==' ')
	{
		p->geom.x+=movex;
	}
	else if (grid[checkx][p->geom.y]=='1' || grid[checkx][p->geom.y+BLOCKHEIGHT-1]=='1')
	{
		line1.dead=true;
		p->geom.x+=movex;	
	}
	else if (grid[checkx][p->geom.y]=='2' || grid[checkx][p->geom.y+BLOCKHEIGHT-1]=='2')
	{
		line2.dead=true;
		p->geom.x+=movex;	
	}
	else if ((grid[checkx][p->geom.y]=='w' || grid[checkx][p->geom.y]==' ' ) 
	      && (grid[checkx][p->geom.y+BLOCKHEIGHT-1]=='w' || grid[checkx][p->geom.y+BLOCKHEIGHT-1]==' '))
	{
		// this is used in the intro. maybe some place else too in the future.
		// should it be merged into the first line above? maybe.
		p->geom.x+=movex;
	}
	else
	{
		p->xdelta=-p->xdelta;
		if (p->image==spriteimage)
			p->image=spritemirrorimage;
		else
			p->image=spriteimage;
	}

	if (movey>0) checky = newy+BLOCKHEIGHT;
	else checky = newy;
	
	if (grid[p->geom.x][checky]==' ' && grid[p->geom.x+BLOCKWIDTH-1][checky]==' ')
	{
		p->geom.y+=movey;
	}
	else if (grid[p->geom.x][checky]=='1' || grid[p->geom.x+BLOCKWIDTH-1][checky]=='1')
	{
		//printf("Hit 1\n");
		line1.dead=true;
		p->geom.y+=movey;		
	}
	else if (grid[p->geom.x][checky]=='2' || grid[p->geom.x+BLOCKWIDTH-1][checky]=='2')
	{
		//printf("Hit 2\n");
		line2.dead=true;
		p->geom.y+=movey;		
	}
	else if ((grid[p->geom.x][checky]=='w' || grid[p->geom.x][checky]==' ') 
	      && (grid[p->geom.x+BLOCKWIDTH-1][checky]=='w' || grid[p->geom.x+BLOCKWIDTH-1][checky]==' '))
	{
		// this is used in the intro. maybe some place else too in the future.
		// should it be merged into the first line above? maybe.
		p->geom.y+=movey;
	}

	else
	{
		p->ydelta=-p->ydelta;
	}

	markgrid(p->geom.x,p->geom.y,BLOCKWIDTH,BLOCKHEIGHT,'*');
}

int createpenguinbgsave(Penguin* p)
{	
	SDL_Surface* tmpsurface;	
		
	tmpsurface = SDL_CreateRGBSurface(SDL_SWSURFACE,p->geom.w,p->geom.h,screen->format->BitsPerPixel,0,0,0,0);
	// fix -- add error checking
	p->bgsave = SDL_DisplayFormat(tmpsurface);
	if (p->bgsave==NULL)
		p->bgsave=tmpsurface;
	else
		SDL_FreeSurface(tmpsurface);

	return 0;
}

