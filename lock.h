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
* with this program; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/


#ifndef LOCK_H
#define LOCK_H

// fix -- should this stuff go in win32_compatibility.h? it's a tradeoff
// between putting stuff closer to where it's used and isolating platform-
// specific stuff

#ifdef WIN32
  #include <windows.h>
  #define FILE_DESC	 	HANDLE
  #define INVALID_FILE_DESC	INVALID_HANDLE_VALUE
#else
  #define FILE_DESC	 	int
  #define INVALID_FILE_DESC	-1
#endif

extern int lock(FILE_DESC fd);
extern int unlock(FILE_DESC fd);

extern FILE_DESC openlockfile(char* fname);
extern void closelockfile(FILE_DESC f);

#endif /* LOCK_H */
