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
* with this program; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include "icebreaker.h"
#include "globals.h"
#include "options.h"
#include "sound.h"
#include "themes.h"

ThemeColorsType color;
SDL_Surface* spriteimage=NULL;
SDL_Surface* spritemirrorimage=NULL;

static int setrandomtheme();
static void setdefaultcolors();
static SDL_Surface* loadsprite(char* themename, char* imagefile);

static int alphasortstringarray(const void *a, const void *b);

int getthemenames(char*** themenamelist)
{
	int s;
	int numthemes;
	int listsize=8;
	char **listtemp;
	
	struct dirent* dirtemp;
	DIR* themedir;

	*themenamelist=malloc(sizeof(char*)*listsize);
	if (themenamelist==NULL)
	{	
		fprintf(stderr,"Error: Couldn't get memory to get list of themes. That's bad.\n");
		return -2;
	}

	themedir=opendir(DATAPREFIX);
	if (themedir==NULL)
	{
		fprintf(stderr, "Couldn't list data directory. This is going to be a problem.\n");
		return -1;
	}
	

	numthemes=0;
	while ((dirtemp = readdir(themedir)) != NULL)
	{
		if (strlen(dirtemp->d_name) <= strlen(THEMEFILEEXTENSION))
		{
			// filename too short
			
			// this space intentionally left blank
		}
		else if (strcasecmp( THEMEFILEEXTENSION, dirtemp->d_name+(strlen(dirtemp->d_name)-strlen(THEMEFILEEXTENSION)) ))
		{
			// wrong extension
			
			// this space intentionally left blank
		}
		else if (strlen(dirtemp->d_name)-strlen(THEMEFILEEXTENSION)>MAXTHEMENAMELENGTH)
		{
			fprintf(stderr, "Theme name %s is too long.\n"
			                "For display reasons, the maximum allowed is %d + %s.\n",dirtemp->d_name,MAXTHEMENAMELENGTH,THEMEFILEEXTENSION);
		}
		else
		{ // Valid theme name found

			if (numthemes>=listsize)
			{
				// trying to put numthemes+1 themes in a listsize-sized bucket
				// so double the size of the bucket.
				listsize *= 2;
				listtemp = (char **) realloc (*themenamelist,sizeof(char*)*listsize);	
				if (listtemp == NULL)
				{
					fprintf(stderr, "Warning: No more memory for theme names.\n");
					break;
				}
				*themenamelist = listtemp;
					
			}	

			s=strlen(dirtemp->d_name)-strlen(THEMEFILEEXTENSION)+1;
			(*themenamelist)[numthemes]=malloc(s);
			if ((*themenamelist)[numthemes]==NULL) 
			{
				fprintf(stderr, "Error: couldn't get memory for theme name entry '%s' (#%d).\n"
				                "Trying to continue with what we've got.\n",dirtemp->d_name,numthemes);
			}
			else
			{
				snprintf((*themenamelist)[numthemes],s,"%s",dirtemp->d_name);
				numthemes++;
			}
		}
	}

	closedir(themedir);

	qsort(*themenamelist,numthemes,sizeof(**themenamelist),alphasortstringarray);

	return numthemes;	
}

void freethemenames(char*** themenamelist,int numthemes)
{
	int i;
	for (i=0;i<numthemes;i++)
	{
		free((*themenamelist)[i]);
	}
	if (numthemes>0) free(*themenamelist);
}

int settheme(char* themename)
{
	int rc=0;
	int i;
	FILE* themefile;
	char linebuf[300];
	char themefilename[256]; // fix -- use defined OS constant (here and throughout this function)
	char optbuf[21];
	char valbuf[256];
	char loadfilebuf[256];
	int r,g,b;
	ThemeColorsType foundcolor;
	
	if (!strcmp(themename,"random"))
	{
		return setrandomtheme(); // whooo recursion!
	}
	
	setdefaultcolors();
	if (spritemirrorimage==spriteimage) spritemirrorimage=NULL; // don't want to double free!
	if (spriteimage!=NULL)
	{
		SDL_FreeSurface(spriteimage);
		spriteimage=NULL;
	}
	if (spritemirrorimage!=NULL)
	{
		SDL_FreeSurface(spritemirrorimage);
		spritemirrorimage=NULL;
	}
	
	foundcolor.background      = 0;
	foundcolor.normaltext      = 0;
	foundcolor.gridline        = 0;
	foundcolor.gridhighlight   = 0;
	foundcolor.line1           = 0;
	foundcolor.line2           = 0;
	foundcolor.menuhighlight   = 0;
	foundcolor.gameovertext    = 0;
	foundcolor.scorescrolltext = 0;
	foundcolor.bonusscrolltext = 0;
	foundcolor.textentrybox    = 0;
	foundcolor.textentrytext   = 0;
	foundcolor.copyrighttext   = 0;

	foundcolor.boardfillminr = 0;
 	foundcolor.boardfillming = 0;
	foundcolor.boardfillminb = 0;
	foundcolor.boardfillmaxr = 0;
	foundcolor.boardfillmaxg = 0;
	foundcolor.boardfillmaxb = 0;

	foundcolor.spritetransparent = 0;
	
	snprintf(themefilename, 256,"%s/%s%s",DATAPREFIX,themename,THEMEFILEEXTENSION);
	
	themefile=fopen(themefilename,"r");
	if (themefile==NULL)
	{
		// FIX -- ??? question: should this actually change the
		// current setting, or is leaving it at the currently-unavailable
		// value the right thing to do?
		fprintf(stderr, "Warning: Can't open theme file %s.\n"
		                "Using default look and sound.\n",themefilename);
		rc=-18;		               
	}
	else
	{
		while(fgets(linebuf,300,themefile))
		{
			for (i=0;i<300;i++)
			{	
				if (linebuf[i]=='\0' || linebuf[i]==' ' || linebuf[i]=='\t') break;
				linebuf[i]=tolower(linebuf[i]);
			}
			
			if (sscanf(linebuf,"%20s %5d %5d %5d",optbuf,&r,&g,&b)==4)
			{
				//printf("N: %s = %d,%d,%d\n",optbuf,r,g,b);
				
				if (r<0 || r>255)
				{
					fprintf(stderr,"Warning: invalid red value for %s in theme %s\n",optbuf,themename);
					r=0;
				}
				if (g<0 || g>255)
				{
					fprintf(stderr,"Warning: invalid green value for %s in theme %s\n",optbuf,themename);
					g=0;
				}
				if (b<0 || b>255)
				{
					fprintf(stderr,"Warning: invalid blue value for %s in theme %s\n",optbuf,themename);
					b=0;
				}
				
				if (!strcmp(optbuf,"background"))
				{
					color.background=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.background++;
				}
				else if (!strcmp(optbuf,"normaltext"))
				{
					color.normaltext=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.normaltext++;
				}
				else if (!strcmp(optbuf,"boardfillmin"))
				{
					color.boardfillminr=r;
					color.boardfillming=g;
					color.boardfillminb=b;
					foundcolor.boardfillminr++;
					foundcolor.boardfillming++;
					foundcolor.boardfillminb++;
					
				}
				else if (!strcmp(optbuf,"boardfillmax"))
				{
					color.boardfillmaxr=r;
					color.boardfillmaxg=g;
					color.boardfillmaxb=b;
					foundcolor.boardfillmaxr++;
					foundcolor.boardfillmaxg++;
					foundcolor.boardfillmaxb++;
				}
				else if (!strcmp(optbuf,"gridline"))
				{
					color.gridline=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.gridline++;
				}
				else if (!strcmp(optbuf,"gridhighlight"))
				{
					color.gridhighlight=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.gridhighlight++;
				}
				else if (!strcmp(optbuf,"line1"))
				{
					color.line1=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.line1++;
				}
				else if (!strcmp(optbuf,"line2"))
				{
					color.line2=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.line2++;
				}
				else if (!strcmp(optbuf,"menuhighlight"))
				{
					color.menuhighlight=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.menuhighlight++;

				}
				else if (!strcmp(optbuf,"gameovertext"))
				{
					color.gameovertext=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.gameovertext++;
				}
				else if (!strcmp(optbuf,"scorescrolltext"))
				{
					color.scorescrolltext=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.scorescrolltext++;
				}
				else if (!strcmp(optbuf,"bonusscrolltext"))
				{
					color.bonusscrolltext=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.bonusscrolltext++;
				}
				else if (!strcmp(optbuf,"textentrybox"))
				{
					color.textentrybox=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.textentrybox++;

				}
				else if (!strcmp(optbuf,"textentrytext"))
				{
					color.textentrytext=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.textentrytext++;
				}
				else if (!strcmp(optbuf,"copyrighttext"))
				{
					color.copyrighttext=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.copyrighttext++;

				}
				else if (!strcmp(optbuf,"spritetransparent"))
				{
					color.spritetransparent=SDL_MapRGB(screen->format,r,g,b);
					foundcolor.spritetransparent++;
				}
			}
			else if (sscanf(linebuf,"%20s %255s",optbuf,valbuf)==2)
			{
				//printf("S: %s = %s\n",optbuf,valbuf);
				if (!strcmp(optbuf,"spritebitmap"))
				{
					if (strcasecmp( ".bmp", valbuf+(strlen(valbuf)-strlen(".bmp")) ))
					{
						fprintf(stderr,"Warning: Sprite bitmap file in theme %s does not end with '.bmp'.\n"
						               "We'll try to load it anyway, but don't be surpised if there's a problem.\n"
						               "(Filename is %s)\n",themename,valbuf);
					}
					snprintf(loadfilebuf, 256,"%s/%s",DATAPREFIX,valbuf);
					spriteimage=loadsprite(themename,loadfilebuf);
				}
				else if (!strcmp(optbuf,"spritemirrorbitmap"))
				{
					if (strcasecmp( ".bmp", valbuf+(strlen(valbuf)-strlen(".bmp")) ))
					{
						fprintf(stderr,"Warning: Sprite bitmap file in theme %s does not end with '.bmp'.\n"
						               "We'll try to load it anyway, but don't be surpised if there's a problem.\n"
						               "(Filename is %s)\n",themename,valbuf);
					}
					snprintf(loadfilebuf, 256,"%s/%s",DATAPREFIX,valbuf);
					spritemirrorimage=loadsprite(themename,loadfilebuf);
				}
				else if (!strcmp(optbuf,"soundouch"))
				{
					if (strcasecmp( ".wav", valbuf+(strlen(valbuf)-strlen(".wav")) ))
					{
						fprintf(stderr,"Warning: Ouch sound file in theme %s does not end with '.wav'.\n"
						               "We'll try to load it anyway, but don't be surpised if there's a problem.\n"
						               "(Filename is %s)\n",themename,valbuf);
					}
					snprintf(loadfilebuf, 256,"%s/%s",DATAPREFIX,valbuf);
					loadsounds(themename,loadfilebuf,NULL);
				}
				else if (!strcmp(optbuf,"soundcrash"))
				{
					if (strcasecmp( ".wav", valbuf+(strlen(valbuf)-strlen(".wav")) ))
					{
						fprintf(stderr,"Warning: Crash sound file in theme %s does not end with '.wav'.\n"
						               "We'll try to load it anyway, but don't be surpised if there's a problem.\n"
						               "(Filename is %s)\n",themename,valbuf);
					}
					snprintf(loadfilebuf, 256,"%s/%s",DATAPREFIX,valbuf);
					loadsounds(themename,NULL,loadfilebuf);
				}
			}
			
		}	
		fclose(themefile);
	}


	if (foundcolor.background == 0)
		{ rc--; fprintf(stderr,"Warning: Background not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.normaltext == 0)
		{ rc--; fprintf(stderr,"Warning: NormalText not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.boardfillminr == 0)
		{ rc--; fprintf(stderr,"Warning: BoardFillMin not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.boardfillmaxr == 0)
		{ rc--; fprintf(stderr,"Warning: BoardFillMax not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.gridline == 0)
		{ rc--; fprintf(stderr,"Warning: Gridline not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.gridhighlight == 0)
		{ rc--; fprintf(stderr,"Warning: GridHighlight not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.line1 == 0)
		{ rc--; fprintf(stderr,"Warning: Line1 not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.line2 == 0)
		{ rc--; fprintf(stderr,"Warning: Line2 not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.menuhighlight == 0)
		{ rc--; fprintf(stderr,"Warning: MenuHighlight not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.gameovertext == 0)
		{ rc--; fprintf(stderr,"Warning: GameoverText not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.scorescrolltext == 0)
		{ rc--; fprintf(stderr,"Warning: ScorescrollText not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.bonusscrolltext == 0)
		{ rc--; fprintf(stderr,"Warning: BonusscrollText not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.textentrybox == 0)
		{ rc--; fprintf(stderr,"Warning: TextentryBox not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.textentrytext == 0)
		{ rc--; fprintf(stderr,"Warning: TextentryText not correctly defined in theme %s. Using default.\n",themename); }
	if (foundcolor.copyrighttext == 0)
		{ rc--; fprintf(stderr,"Warning: CopyrightText not correctly defined in theme %s. Using default.\n",themename); }

	if (foundcolor.spritetransparent == 0)
	{ 
		rc--;
		fprintf(stderr,"Warning: SpriteTransparent not correctly defined in theme %s.\n",themename); 
		color.spritetransparent=SDL_MapRGB(screen->format,255,  0,  0);
	}

	
	if (color.boardfillmaxr<color.boardfillminr)
	{
		color.boardfillmaxr=color.boardfillminr;
		fprintf(stderr,"Warning: max red value less than min value in theme %s\n",themename);
	}
	if (color.boardfillmaxg<color.boardfillming)
	{
		color.boardfillmaxg=color.boardfillming;
		fprintf(stderr,"Warning: max green value less than min value in theme %s\n",themename);
	}
	if (color.boardfillmaxb<color.boardfillminb)
	{
		color.boardfillmaxb=color.boardfillminb;
		fprintf(stderr,"Warning: max blue value less than min value in theme %s\n",themename);
	}
	

	if (spriteimage==NULL)
	{
		rc--;
		fprintf(stderr,"Warning: SpriteBitmap not correctly defined in theme %s. Using default.\n",themename);
		fprintf(stderr,"         Also using default SpriteTransparent.\n");	
		color.spritetransparent=SDL_MapRGB(screen->format,255,  0,  0);
		spriteimage=loadsprite(themename,DATAPREFIX "/" PENGUINBMPFILE);
		if (spriteimage==NULL)
		{ // uh oh -- no sprite at all. fix -- do something?
			fprintf(stderr,"Error: couldn't load any sprite at all!!!\n");
			rc=-255;
		}
	}
	
	if (spritemirrorimage==NULL)
	{
		// we don't warn about this -- we just use the
		// main sprite image and go on silently
		spritemirrorimage=spriteimage;
	}
		
	return rc;
}

int getthemenumber(char** themenamelist,int numthemes,char* themename)
{
	int i;
	for (i=0;i<numthemes;i++)
	{
		if (!strcmp(themenamelist[i],themename))
			return i;
	}
	return -1;
}

int setrandomtheme()
{
	char themename[MAXTHEMENAMELENGTH];
	char** themelist;
	int themecount;
	
	themecount=getthemenames(&themelist);
	if (getthemenumber(themelist,themecount,"random")!=-1)
	{
		fprintf(stderr,"Hey! You can't have a theme named random! Please remove the random.ibt\n"
		        "file from your themes directory.\n");
		snprintf(themename,MAXTHEMENAMELENGTH+1,"linux");
	}
	else
	{
		snprintf(themename,MAXTHEMENAMELENGTH+1,themelist[random() %themecount]);
	}
	freethemenames(&themelist,themecount);
	return settheme(themename);
}

void setdefaultcolors()
{
	color.background      = SDL_MapRGB(screen->format,  0, 64,128);
	color.normaltext      = SDL_MapRGB(screen->format,255,255,255);
	color.gridline        = SDL_MapRGB(screen->format,192,192,192);
	color.gridhighlight   = SDL_MapRGB(screen->format,192,192,192);
	color.line1           = SDL_MapRGB(screen->format,  0,  0,  0);
	color.line2           = SDL_MapRGB(screen->format,192,  0, 64);
	color.menuhighlight   = SDL_MapRGB(screen->format,240,240,240);
	color.gameovertext    = SDL_MapRGB(screen->format,192,  0, 64);
	color.scorescrolltext = SDL_MapRGB(screen->format,  0,  0,  0);
	color.bonusscrolltext = SDL_MapRGB(screen->format,192,  0, 64);
	color.textentrybox    = SDL_MapRGB(screen->format,  0,  0,  0);
	color.textentrytext   = SDL_MapRGB(screen->format,255,255,255);
	color.copyrighttext   = SDL_MapRGB(screen->format,192,192,192);

	color.boardfillminr = 224;
 	color.boardfillming = 224;
	color.boardfillminb = 224;
	color.boardfillmaxr = 255;
	color.boardfillmaxg = 255;
	color.boardfillmaxb = 255;
}

SDL_Surface* loadsprite(char* themename, char* imagefile)
{
	SDL_Surface* imageloadtemp1;
	SDL_Surface* imageloadtemp2;

	imageloadtemp1 = SDL_LoadBMP(imagefile);
	if (imageloadtemp1==NULL)
	{
		fprintf(stderr, "Couldn't load image %s.\n"
		                "SDL error: "
		                "%s\n\n", imagefile, SDL_GetError());
		return NULL;
	}
	
	if (imageloadtemp1->w > BLOCKWIDTH || imageloadtemp1->h > BLOCKHEIGHT)
	{
		fprintf(stderr, "Error: Image file %s too large!\n", imagefile);
		return NULL;
	}
	if (imageloadtemp1->w < BLOCKWIDTH-2 || imageloadtemp1->h < BLOCKHEIGHT-2)
	{
		fprintf(stderr, "Error: Image file %s too small!\n", imagefile);
		return NULL;
	}

	imageloadtemp2 = SDL_DisplayFormat(imageloadtemp1);
	if (imageloadtemp2==NULL)
	{
		fprintf(stderr, "Warning: couldn't convert %s to screen format.\n"
		                "Perhaps we ran out of memory? We'll live, but there will be a performance hit.\n",
		                imagefile);
		imageloadtemp2 = imageloadtemp1;
	}
	else
	{		
		SDL_FreeSurface(imageloadtemp1);
	}

	if (SDL_SetColorKey(imageloadtemp2, SDL_SRCCOLORKEY, color.spritetransparent))
	{
		if (themename!=NULL)
		{
			fprintf(stderr, "Warning: couldn't set colorkey for %s.\n"
			                "This is probably a problem in the '%s' theme.\n",
			                 imagefile,themename);
			return NULL;
		}
		else
		{
			fprintf(stderr, "Warning: couldn't set colorkey for %s.\n"
			                "Since that's the default image, this probably means there's a serious problem.\n",
			                 imagefile);
			return NULL;
		}
	}

	return imageloadtemp2;
}

static int alphasortstringarray(const void *a, const void *b)
{
	return strcoll ( *(const char **)a, *(const char **)b);
}
