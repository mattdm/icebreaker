/*
* IceBreaker
* Copyright (c) 2000-2020 Matthew Miller <mattdm@mattdm.org>
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
#include <string.h>
#include <stdlib.h>
#include "icebreaker.h"
#include "laundry.h"
#include "grid.h"
#include "penguin.h"
#include "globals.h"
#include "themes.h"

SDL_Surface* gridsave;
char grid[WIDTH][HEIGHT];


static char maskgrid[WIDTH][HEIGHT];

// kludge-o-rama
static long rcount;
#define MAXRCOUNT 80000


int initgrid()
{
	// fix -- add error checking	
	gridsave = SDL_CreateRGBSurface(SDL_SWSURFACE,WIDTH,HEIGHT,screen->format->BitsPerPixel,0,0,0,0);
	return 0;
}

void quitgrid()
{
	SDL_FreeSurface(gridsave);
}

void drawgridblocks()
{
	int r,g,b;
	SDL_Rect tmprect;

	SDL_FillRect(gridsave,NULL,color.background);


	for (tmprect.x=BORDERLEFT;tmprect.x<BORDERRIGHT;tmprect.x+=BLOCKWIDTH)
		for (tmprect.y=BORDERTOP;tmprect.y<BORDERBOTTOM;tmprect.y+=BLOCKHEIGHT)
		{
			r = (random() % (color.boardfillmaxr-color.boardfillminr+1))+color.boardfillminr;

			if (color.boardfillminr == color.boardfillming && color.boardfillmaxr == color.boardfillmaxg)
				g=r;
			else
				g = (random() % (color.boardfillmaxg-color.boardfillming+1))+color.boardfillming;

			if (color.boardfillminr == color.boardfillminb && color.boardfillmaxr == color.boardfillmaxb)
				b=r;		
			else if (color.boardfillming == color.boardfillminb && color.boardfillmaxg == color.boardfillmaxb)
				b=g;
			else
				b = (random() % (color.boardfillmaxb-color.boardfillminb+1))+color.boardfillminb; 
		
			//if (grid[tmprect.x][tmprect.y]==' ' || grid[tmprect.x][tmprect.y]=='w')
			if (grid[tmprect.x][tmprect.y]!='.' && grid[tmprect.x][tmprect.y]!='-' && grid[tmprect.x][tmprect.y]!='|')
			{
				tmprect.w=BLOCKWIDTH; tmprect.h=BLOCKHEIGHT;
				SDL_FillRect(gridsave,&tmprect,color.gridline);
				if (color.gridhighlight==color.gridline)
				{
					tmprect.w=BLOCKWIDTH-1; tmprect.h=BLOCKHEIGHT-1; // this makes the gridline show up
					SDL_FillRect(gridsave,&tmprect,SDL_MapRGB(screen->format, r, g, b));
				}
				else
				{
					tmprect.w=BLOCKWIDTH-1; tmprect.h=BLOCKHEIGHT-1;
					SDL_FillRect(gridsave,&tmprect,color.gridhighlight);

					tmprect.w=BLOCKWIDTH-2; tmprect.h=BLOCKHEIGHT-2;
					// FIX! this is kludgy! changing the index variable on the fly bad bad bad. (even if I do put it back!)
					tmprect.x++; tmprect.y++;
					SDL_FillRect(gridsave,&tmprect,SDL_MapRGB(screen->format, r, g, b));
					tmprect.x--; tmprect.y--;
				
				}
			}
		}
	
	tmprect.x=BORDERLEFT; tmprect.y=BORDERTOP;
	tmprect.w=PLAYWIDTH; tmprect.h=PLAYHEIGHT;
	SDL_BlitSurface(gridsave, &tmprect, screen, &tmprect);
}

void markgrid(int x, int y, int w, int h, char fillchar)
{
	int i;
	
	// Optimizing this routine much further seems about impossible to me
	// but if you know how to do it, let me know. This is by far the
	// most frequently-called function in the whole game. Short of
	// rethinking the collision detection -- which could stand a rethink
	// anyway -- there's not much that can be done.
	
	for (i=x;i<x+w;i++)
		memset(&grid[i][y],fillchar,h);
	
	/*
	int i, j;
	for (j=y;j<y+h;j++)
		for (i=x;i<x+w;i++)
			grid[i][j]=fillchar;		
	*/	
}

long countcleared()
{
	int i, j;
	long c;
	c=0;
	for (i=BORDERLEFT;i<BORDERRIGHT;i++)
		for (j=BORDERTOP;j<BORDERBOTTOM;j++)
			if (grid[i][j] == ' ' || grid[i][j] == '*')
				c++;
	//return(100-(c*100/(PLAYWIDTH*PLAYHEIGHT)));
	return(c);
	
}

#ifdef DEBUG
void printboard()
{
	int i, j;
	
	for (j=BLOCKWIDTH/2;j<HEIGHT;j+=BLOCKHEIGHT)
	{
		for (i=BLOCKWIDTH/2;i<WIDTH;i+=BLOCKWIDTH)
		{
			printf("%c ",grid[i][j]);
		}
		printf("\n");
	}
}
#endif

#ifdef DEBUG
void printwholegrid()
{
	int i, j;
	
	printf ("grid:\n");
	for (j=0;j<HEIGHT;j++)
	{
		for (i=0;i<WIDTH;i++)
		{
			printf("%c ",grid[i][j]);
		}
		printf("\n");
	}
}
#endif

#ifdef DEBUG
void printwholemaskgrid()
{
	int i, j;
	
	printf ("maskgrid:\n");
	for (j=0;j<HEIGHT;j++)
	{
		for (i=0;i<WIDTH;i++)
		{
			printf("%c ",maskgrid[i][j]);
		}
		printf("\n");
	}
}
#endif

void checkempty(int x, int y)
{
	//int i,j;
	
	// for debugging...
	
	SDL_Rect tmprect;
	

	// if square isn't empty, just return....
	if (grid[x][y]!=' ') {  return; }


	// it'd be nice to find a way to keep this longer...
	memcpy(maskgrid,grid,WIDTH*HEIGHT);
	

	// penguinsearch at that spot...
	rcount=0;
	if (!penguinsearch(x,y)) // area is clear!
	{
		//printwholemaskgrid();
		

		
		//floodfill(x,y);
		
		// this makes sure x and y are the top left corners of blocks.
		// since the area is empty of penguins, it should be completely
		// safe to use this isntead of floodfill here. really. :)
		squarefill( (((x-BORDERLEFT)/BLOCKWIDTH ) * BLOCKWIDTH ) +BORDERLEFT, (((y-BORDERTOP)/BLOCKHEIGHT) * BLOCKHEIGHT) +BORDERTOP);

		tmprect.w=BLOCKWIDTH; tmprect.h=BLOCKHEIGHT;
		for (tmprect.x=BORDERLEFT;tmprect.x<BORDERRIGHT;tmprect.x+=BLOCKWIDTH)
			for (tmprect.y=BORDERTOP;tmprect.y<BORDERBOTTOM;tmprect.y+=BLOCKHEIGHT)
				if (grid[tmprect.x][tmprect.y]=='.') // clear it!)
				{
					SDL_FillRect(screen,&tmprect,color.background);
					soil(tmprect);
				}
		//printwholegrid();
	}
	/* printf("Search took %ld recursions.\n",rcount); */

	/*
	for (j=0;j<HEIGHT;j+=BLOCKHEIGHT)
	{
		for (i=0;i<WIDTH;i+=BLOCKWIDTH)
		{
			printf("%c ",maskgrid[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	*/
}


int penguinsearch(int i, int j)
{
	int searchval=0;

	rcount++; 
	// kludge! FIX! BAD!
	if (rcount>MAXRCOUNT) // bail
	{
		fprintf(stderr,"Damn. Ran out of recursions.\n");
		return(2);
	}
	
	
	// shouldn't need to check bounds because we're only painting in the
	// middle. and we call this function so much that the time saved
	// is worth it
	//if (i<0 || j<0 || i>=WIDTH || j>=HEIGHT)
	//{
	//	fprintf(stderr,"That shouldn't have happened (penguinsearch)! (%d,%d)\n",i,j);
	//	exit(1);
	//}

          
	if (maskgrid[i][j]==' '
	    || maskgrid[i][j]=='1' || maskgrid[i][j]=='2' || maskgrid[i][j]=='w')  // Ah ha! The nefarious "instant melting ice" bug solved!  NOTE: if more lines are added to the game, add them here too!
	{
		maskgrid[i][j]=',';
		
		// hmmm. the "ice-shelf-collapse" bug *isn't* fixed. :(
		
		searchval=penguinsearch(i+BLOCKWIDTH, j);
		if (!searchval) searchval=penguinsearch(i-BLOCKWIDTH, j);
		if (!searchval) searchval=penguinsearch(i, j-BLOCKHEIGHT);
		if (!searchval) searchval=penguinsearch(i, j+BLOCKHEIGHT);
		
	}
	else if (maskgrid[i][j]=='*') // found a penguin!
	{
		searchval=1;	
	}
	return(searchval);
}


void floodfill(int x, int y)
{
	// shouldn't need to check bounds because we're only painting in the
	// middle. ie (x<0 || y<0 || x>WIDTH || y>HEIGHT) is always false.
	if (grid[x][y]==' ' || grid[x][y]=='1' || grid[x][y]=='2' || grid[x][y]=='w')
	{
		grid[x][y]='.';
		floodfill(x+1, y);
		floodfill(x-1, y);
		floodfill(x, y+1);
		floodfill(x, y-1);
	}
}

void squarefill(int x, int y)
{
	// x and y must be the top left corner of a square, or else this
	// will look silly. and there's no bounds checking!

	if (grid[x][y]==' ' || grid[x][y]=='1' || grid[x][y]=='2' || grid[x][y]=='w')
	{
		markgrid(x,y,BLOCKWIDTH,BLOCKHEIGHT,'.');
		squarefill(x+BLOCKWIDTH, y);
		squarefill(x-BLOCKWIDTH, y);
		squarefill(x, y+BLOCKHEIGHT);
		squarefill(x, y-BLOCKHEIGHT);
	}
}

