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

#ifndef OPTIONS_H
#define OPTIONS_H

typedef enum { SOUNDON,SOUNDOFF } SoundSettingType;
typedef enum { AUTOPAUSEON,AUTOPAUSEOFF } AutoPauseType;
typedef enum { NORMAL, EASY, HARD } GameDifficultyType;
typedef enum { FULLSCREENOFF, FULLSCREENON, FULLSCREENALWAYS, FULLSCREENUNKNOWN } FullscreenSettingType;

typedef struct
{
	SoundSettingType sound;
	AutoPauseType autopause;
	GameDifficultyType difficulty;
	FullscreenSettingType fullscreen;
        char theme[11+MAXTHEMENAMELENGTH];
} GameOptionsType;

typedef struct
{
	int isfullscreen;
	int benchmarkmode;
	int soundsystemworks;
	int themechanged;
} GameFlagsType;

extern GameOptionsType options;
extern GameOptionsType commandline;

extern GameFlagsType gameflags;

void setdefaultoptions(void);
extern int readoptions(void);
extern int writeoptions(void);

extern int parsecommandline(int argc, char** argv);

#endif /* OPTIONS_H */
