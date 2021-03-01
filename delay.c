/*
* IceBreaker
* Copyright (c) 2002-2020 Matthew Miller <mattdm@mattdm.org> and 
*  Enrico Tassi <gareuselesinge@infinito.it>
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
#include <stdio.h>
#include "icebreaker.h"


// oh, if only we could sleep for less than 10ms on  
// intel. too bad alpha systems suck so much in other
// ways -- they can sleep with 1ms resolution.
// (we could on intel with nanosleep, if we were suid root...)
int delaytil(int futuretime)
{
	int slept=false;

	// first, soft-wait for 10ms intervals until there's less than 10ms
	// left to go
	while (futuretime-10 >= SDL_GetTicks())
	{
		SDL_Delay(10);
		slept=true;
	}


	// and then busy-wait for the leftover bits

	#ifdef FUZZYFPS
	if (!slept)
	#endif
	while (futuretime > SDL_GetTicks())
	{
		// busy loop!
		// FIX -- calculate PI or something interesting here. :)
	}
	
	
	return 0; // fix -- return time actually slept?
}

