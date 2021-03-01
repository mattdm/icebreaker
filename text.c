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

#include <SDL.h>
#include <string.h>

#include "icebreaker.h"
#include "text.h"
#include "globals.h"

static char * letterindex;
static char * letterdata[6];


void inittext()
{
	letterindex = " AAAAA BBBBB CCCCC DDDDD EEEEE FFFFF GGGGG HHHHH III JJJJJ KKKK LLLLL MMMMM NNNNN OOOOO PPPPP QQQQQ RRRRR SSSSS TTTTT UUUUU VVVVV WWWWW XXXXX YYYYY ZZZZZ aaaaa bbbbb cccc ddddd eeeee fffff ggggg hhhhh i jjjj kkkk lll mmmmm nnnnn ooooo ppppp qqqqq rrrr sssss tttt uuuuu vvvvv wwwww xxxx yyyyy zzzz 00000 11 22222 33333 44444 55555 66666 77777 88888 99999 ~~~~~    ``     !    @@@@   #####  $$$$$  %%%%%  ^^^^^  &&&&&  *****    ((   ))    _____  -----  +++++  =====   {{{     [[[   }}}   ]]]      |     :     ;;      ''   <<<<<<   ,,   >>>>>>    .    ????   /////    \"\"\" \\\\\\\\\\";
	letterdata[0]="   #   ####   ###  ####  ##### #####  #### #   # ###   ### #  # #     #   # #   #  ###  ####   ###  ####   #### ##### #   # #   # #   # #   # #   # #####       #              #         ###       #     #    # #    ##                                            #                                      ###   # ####  ####     ## #####  ###  #####  ###   ###   #       #      #     ##     # #    ###   ##  #    #     ##      #       #   #                     #              #     ###   #     ###      #                    #       ##        ##             ###        #    # # #    ";
	letterdata[1]="  # #  #   # #   # #   # #     #     #     #   #  #      # # #  #     ## ## ##  # #   # #   # #   # #   # #       #   #   # #   # # # #  # #   # #     #   #### ####   ###  ####  ###   #     #### ####         # ##  #  ## #  # ##   ###  ####   #### # ##  #### #### #   # #   # #   # #  # #   # #### #   # ##     #     #   # # #     #         # #   # #   # # # #     #     #    # ##   #####  # #    ## #    # #   #      # # #    #     #                    #    #####    #      #      #      #      #     #      #      #      ##            ##              #      #     # #  #   ";
	letterdata[2]=" #   # ####  #     #   # ####  ####  #  ## #####  #      # ##   #     # # # # # # #   # ####  #   # ####   ###    #   #   #  # #  # # #   #     #     #   #   # #   # #    #   # ## ## ####  #   # #   # #    # ##    #  # # # ##  # #   # #   # #   # ##   ###    #   #   #  # #  # # #  ##  #   #   #  #   #  #  ###   ###   #  # ####  ####     #   ###   ####    #            #    # ##    # #    ###     #    #   #   ## #   ###     #     #           #####  #####          ##      #      ##     #      #                        ##                ##          ##      #            #  ";
	letterdata[3]=" ##### #   # #   # #   # #     #     #   # #   #  #  #   # # #  #     # # # #  ## #   # #     #   # #  #      #   #   #   #  # #  ## ##  # #    #    #    #  ## #   # #    #   # ###    #     #### #   # #    # # #   #  # # # #   # #   # #   # #   # #      ###  #   #  ##  # #  # # #  ##   ####  #   #   #  # #         # #####     # #   #   #   #   #     #                      #      #####    # #   # ##         #  #   # # #    #     #                    #    #####    #      #      #      #      #     #      #             ##      #     ##      #            #              # ";
	letterdata[4]=" #   # ####   ###  ####  ##### #      #### #   # ###  ###  #  # ##### #   # #   #  ###  #      ###  #   # ####    #    ###    #   #   # #   #   #   #####  ## # ####   ###  ####  ###   #        # #   # # #  # #  # ### #   # #   #  ###  ####   #### #    ####    ##  ## #   #    # #  #  #     # ####  ###   # ##### ####      # ####   ###   #     ###   ###                  #     ##     # #    ###   #  ##          ## #    #       #   #                     #              #     ###   #     ###      #            #               ##    #   ##        #     #     #                #";
        letterdata[5]="                                                                                                 ##                                                                                           ###           ##                             #         #                                         ###                                                                                                                                                   #####                                                                 #                     #                                            ";
}

int gettextwidth(int size, const char * text)
{
	unsigned int l;
	int width=0;
		
	for (l=0; l<strlen(text); l++)
	{
		width += getletterwidth(size,*(text+l));
		width +=  CHARSPACE;
	}


	// spacing after last letter is not considered		
	width -= CHARSPACE ;

	//fprintf(stderr,"Len of '%s' with size %d is %d pixels.\n",text,size,width);
	
	return width;
}

int getletterwidth(int size, const char letter)
{
	int i;
	char * indexpointer;
	int width=0;
		
	indexpointer=index(letterindex,letter);
	if (indexpointer!=NULL)
		for (i=indexpointer-letterindex; *(letterindex+i)==letter; i++)
		{
			width+= size;
		}

	return width;
}

void puttext(int x, int y, int size, Uint32 color, char * text)
{
	sputtext(screen,x,y,size,color,text);
}

void sputtext(SDL_Surface* scr,int x, int y, int size, Uint32 color, char * text)
{

	SDL_Rect tmprect;
	unsigned int l;
	int i,j;
	char * indexpointer;
	unsigned char c;
	int w,startx;
	
	if (size>1)
	{
		tmprect.h=(int)(size/2.0+0.5);
		tmprect.w=(int)(size/2.0+0.5);
	}
	else
	{
		tmprect.h=1; tmprect.w=1;
	}
	startx=x;
		
	for (l=0; l<strlen(text); l++)
	{
		c=*(text+l);
		indexpointer=index(letterindex,c);
		if (indexpointer!=NULL)
		{
			w=0;
			for (i=indexpointer-letterindex; *(letterindex+i)==c; i++)
			{ // scan through index for location of letter.
				for (j=0;j<CHARHEIGHT;j++)
				{
					if (*(letterdata[j]+i) != ' ') // hmm. maybe there's a better way to deal with spaces?
					{
						tmprect.x=startx+w;
						tmprect.y=y+j*size;
						SDL_FillRect(scr,&tmprect,color);
					}	
				}
				w+=size;
			}
			startx+=w+CHARSPACE;
		}
	}
}

