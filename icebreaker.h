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

// fix -- move most of this to be config.h, and make the current globals.h
// be icebreaker.h

#ifndef ICEBREAKER_H
#define ICEBREAKER_H

/************************************************************************/
/* Versions and so forth -- you probably shouldn't change this. */

#define VERSION 1.9.9
#define VERMAJOR 1
#define VERMINOR 9
#define VERSUB 9

#ifndef DEVELRELEASE
	#define DEVELRELEASE VERMINOR & 1
#endif

/************************************************************************/
/* Platform-specific stuff */


#ifdef __MINGW32__
  #ifndef WIN32
    #define WIN32
  #endif
#endif


#ifdef WIN32

  #include "win32_compatibility.h"

  #ifndef OPTIONFILE
    #define OPTIONFILE "icebreaker.cfg"
  #endif

  #define PENGUINICONFILE "penguinicon_32.bmp"
  
  #define HISCORELOCKFILE "./lockhelper.lck"

  // HISCOREPREFIX and DATAPREFIX should probably be ".". Set in Makefile.w32
#endif


#ifdef __BEOS__

  #define NEEDCHANGETOARGV0PATH

  #ifndef OPTIONFILE
    #define OPTIONFILE "config/settings/icebreaker.cfg"
  #endif

  // HISCOREPREFIX and DATAPREFIX should probably be ".". Set in Makefile.beos
#endif


#ifdef __MACOSX__

  #error "Mac OS probably doesn't actually work yet -- help wanted."

  #define ONEBUTTONMICEARESILLY

  // HISCOREPREFIX and DATAPREFIX should probably be ".". Set in Makefile.osx
#endif


/************************************************************************/
/* and here's the defaults */



#ifndef DATAPREFIX 
  #define DATAPREFIX "/usr/local/share/icebreaker"
#endif

#ifndef OPTIONFILE
  #define OPTIONFILE ".icebreaker"
#endif  

#ifndef HISCOREFILE
  #define HISCOREFILE ".icebreaker.scores"
#endif

#ifndef HISCORELOCKFILE
  #define HISCORELOCKFILE HISCOREFILE
#endif

#ifndef PENGUINICONFILE
  #define PENGUINICONFILE "icebreaker_48.bmp"
#endif

/************************************************************************/
/* themes stuff */

#define THEMEFILEEXTENSION ".ibt"
// this isn't because of filesystem restrictions, although it might help
// with portability. It's because of display space in the options menu. 
#define MAXTHEMENAMELENGTH 7

// these are used if no themes are found -- probably could remove this
// completely now that themes are working well.
#define SNDFILEOUCH "ouch.wav"
#define SNDFILEBREAK "crash.wav"
#define PENGUINBMPFILE "penguin.bmp"


/************************************************************************/
/* Changing these will significantly affect game play. */

#define BLOCKWIDTH 14
#define BLOCKHEIGHT 14

#define COLS 32
#define ROWS 20

#define PLAYWIDTH (COLS*BLOCKWIDTH)
#define PLAYHEIGHT (ROWS*BLOCKHEIGHT)

#define MARGINTOP 39
#define MARGINBOTTOM 38
#define MARGINLEFT 26
#define MARGINRIGHT 26

#define FULLWIDTH 1920
#define FULLHEIGHT 1080
#define FULLTOPMARGIN 80
#define FULLLEFTMARGIN ( (FULLWIDTH - WIDTH)/2 )

#define WIDTH (MARGINLEFT+PLAYWIDTH+MARGINRIGHT)
#define HEIGHT (MARGINTOP+PLAYHEIGHT+MARGINBOTTOM)

#define BORDERTOP    MARGINTOP
#define BORDERBOTTOM (MARGINTOP+PLAYHEIGHT)
#define BORDERLEFT   MARGINLEFT
#define BORDERRIGHT  (MARGINLEFT+PLAYWIDTH)

#define VIDEODEPTH 16

#define MAXPENGUINS 100

#define PENGUINSPEED 2

#define LINESPEED 2

#define LINEMAXSTUCK 750

// fps = 1000/MSECPERFRAME (not counting fuzziness, which will round this
// down to the nearest 10)
#define MSECPERFRAME 11

#define FUZZYFPS

#define PERCENTREQUIRED 80
#define PERCENTBONUS 80
#define PERCENTEXTRABONUS 85


#define KEYBOARDARROWTHROTTLER 5
#define KEYBOARDARROWACCELPOINT 5

/************************************************************************/
/* and, always useful: */
#ifndef true
#define true -1
#endif

#ifndef false
#define false 0
#endif




#endif /* ICEBREAKER_H */
