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
* with this program; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#include <SDL.h>
#include <SDL_mixer.h>
#include "icebreaker.h"
#include "sound.h"
#include "options.h"


static Mix_Chunk *sndouch = NULL;
static Mix_Chunk *sndbreak = NULL;


void initsound()
{
	gameflags.soundsystemworks=false;

	if (commandline.sound==SOUNDOFF)
		return;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO))
	{
		fprintf(stderr, "Can't start sound subsystem: %s.\nContinuing without it.\n", SDL_GetError());
	}
	else if (Mix_OpenAudio(22050, AUDIO_U8, 2, 1024))
	{
		fprintf(stderr, "Can't open audio: %s.\nContinuing without sound support.\n", SDL_GetError());
		fprintf(stderr, "You may see some other errors from system sound libraries. Ignore them.\n");
	}
	else
	{
		gameflags.soundsystemworks=true;
		loadsounds(NULL,DATAPREFIX "/" SNDFILEOUCH,DATAPREFIX "/" SNDFILEBREAK);
	}
}

int loadsounds(char* themename, char* soundouchfile, char* soundcrashfile)
{
	int rc=0;

	if (gameflags.soundsystemworks==false)
		return 0;

	if (soundouchfile!=NULL)
	{
		while (Mix_Playing(-1));
		if (sndouch) { Mix_FreeChunk(sndouch); sndouch = NULL; }
		if ( (sndouch=Mix_LoadWAV(soundouchfile)) == NULL)
		{
			fprintf(stderr, "Error loading " DATAPREFIX "/" SNDFILEOUCH "\n%s\n", SDL_GetError());
			rc--;
		}
	}

	if (soundcrashfile!=NULL)
	{
		while (Mix_Playing(-1));
		if (sndbreak) { Mix_FreeChunk(sndbreak); sndbreak = NULL; }
		if ( (sndbreak=Mix_LoadWAV(soundcrashfile)) == NULL)
		{
			fprintf(stderr, "Error loading " DATAPREFIX "/" SNDFILEBREAK "\n%s\n", SDL_GetError()); // fix -- is "break" or "crash" a better generic term?
			rc--;
		}
	}
	
	return rc;
}
void quitsound()
{
	while (Mix_Playing(-1)); 
	
	if (sndouch) { Mix_FreeChunk(sndouch); sndouch = NULL; }
	if (sndbreak) { Mix_FreeChunk(sndbreak); sndbreak = NULL; }
	
	if (gameflags.soundsystemworks) Mix_CloseAudio();	
}

#define MAXBREAKSOUNDOVERLAP 30
void playsound(SoundSample s)
{
	static Uint32 lastplayedbreak=0;

	if (gameflags.soundsystemworks && options.sound==SOUNDON)
	{
		switch (s)
		{
			case SNDOUCH:
				Mix_PlayChannel(-1, sndouch, 0);
			break;
			case SNDBREAK:
				if (SDL_GetTicks()-lastplayedbreak>=MAXBREAKSOUNDOVERLAP)
				{ // fix -- this is on the right track, but it'd be better
				  //        to some how schedule the sound to be played later.
					lastplayedbreak=SDL_GetTicks();
					Mix_PlayChannel(-1, sndbreak, 0);
				}
			break;
		}
	}
}
