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
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*
*/


#include <SDL.h>
#include "icebreaker.h"
#include "globals.h"
#include "event.h"
#include "options.h"
#include "laundry.h"
#include "text.h"
#include "fullscreen.h"

static void alignmouse(SDL_Event* e);

int pollevent(SDL_Event* e)
{
	int result = SDL_PollEvent(e);
	if ( gameflags.isfullscreen && e != NULL )
		alignmouse(e);
	return result;
}

Uint8 getmousestate(int* mx, int* my)
{
	Uint8 rc;
	rc=SDL_GetMouseState(mx, my);

	if (gameflags.isfullscreen)
	{
		*mx -= (FULLWIDTH - WIDTH) / 2;
		*my -= FULLTOPMARGIN;
	}
	
	return rc;
}

void jumpmouse(int mx, int my)
{	
	if (gameflags.isfullscreen)
		SDL_WarpMouse(mx+(FULLWIDTH - WIDTH) / 2,my+FULLTOPMARGIN);
	else
		SDL_WarpMouse(mx,my);
}

KeyboardActionType translatekeyevent(SDL_Event* e)
{
	if ((e->type != SDL_KEYUP) && (e->type != SDL_KEYDOWN))
		return KEYNONE;
	
	switch (e->key.keysym.sym)
	{
		case SDLK_ESCAPE:
			return KEYCANCEL;
		break;
		
		case SDLK_m:
			return KEYMENU;
		break;
	
		case SDLK_p:
		case SDLK_PAUSE:
			return KEYPAUSE;
		break;

		case SDLK_F1:
			return KEYHELP;
		break;

		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_z:
		case SDLK_COMMA:
		case SDLK_KP0:
			return KEYSTARTLINE;
		break;

		case SDLK_SPACE:
		case SDLK_x:
		case SDLK_PERIOD:
		case SDLK_KP5:
			return KEYSWITCHLINE;
		break;
	
		case SDLK_LEFT:
		case SDLK_h:
		case SDLK_KP4:
			return KEYMOVELEFT;
		break;	

		case SDLK_RIGHT:
		case SDLK_l:
		case SDLK_KP6:
			return KEYMOVERIGHT;
		break;	

		case SDLK_UP:
		case SDLK_k:
		case SDLK_KP8:
			return KEYMOVEUP;
		break;	
	
		case SDLK_DOWN:
		case SDLK_j:
		case SDLK_KP2:
			return KEYMOVEDOWN;
		break;	
		
		case SDLK_KP7:
		case SDLK_HOME:
			return KEYMOVELEFTUP;
		break;
		
		case SDLK_KP9:
		case SDLK_PAGEUP:
			return KEYMOVERIGHTUP;
		break;
		
		case SDLK_KP1:
		case SDLK_END:
			return KEYMOVELEFTDOWN;
		break;
		
		case SDLK_KP3:
		case SDLK_PAGEDOWN:
			return KEYMOVERIGHTDOWN;
		break;

		default:
			return KEYNONE;
		break;
	}

	return KEYNONE;
}

void alignmouse(SDL_Event* e)
{
	
	switch(e->type)
	{
		case SDL_MOUSEMOTION:
			e->motion.x -= (FULLWIDTH - WIDTH) / 2;
			e->motion.y -= FULLTOPMARGIN;
			e->motion.xrel -= (FULLWIDTH - WIDTH) / 2;
			e->motion.yrel -= FULLTOPMARGIN;
		break;
		
		case SDL_MOUSEBUTTONDOWN: // falls through
		case SDL_MOUSEBUTTONUP:
			e->button.x -= (FULLWIDTH - WIDTH) / 2;
			e->button.y -= FULLTOPMARGIN;
		break;		
		
		default:
			return;
	}	
}

