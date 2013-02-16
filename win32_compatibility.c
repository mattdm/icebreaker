/*
* IceBreaker
* Copyright (c) 2000-2002 Matthew Miller <mattdm@mattdm.org> and
*   Enrico Tassi <gareuselesinge@infinito.it>
* 
* <http://www.mattdm.org/icebreaker>
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


/************************************************************************/
/* 
*  This file contains varibles and functions to aid in win32 cross-compiling
*  using mingw32.
*
*  Enrico Tassi <gareuselesinge@infinito.it> contributed the initial work.
*/

#include <stdio.h> 
#include <stdarg.h> 
#include <windows.h>
#include <lmcons.h>
#include "win32_compatibility.h"


#ifdef __MINGW32__

/* This is the structure that getpwuid returns. fix -- how to get username
   in winNT, etc? */
struct passwd pwdwin32_standard={NULL,"*",1,1,"Win32 User",".","command.com"};

/* mingw32's string.h is missing index() */
char *index(const char *s, int c)
{
	while( *s != '\0'&& *s != (char)c ) s++;
	return((char*)s);
}

/* foolish foolish windows. the _snprintf function writes size+1 chars, not
   just size as it is supposed to. */
int snprintf_mingw32kludge(char *str, size_t size, const char *fmt, ...)
{
	int rc;
	va_list args;
	va_start(args, fmt);
	rc=_vsnprintf(str,size-1,fmt,args);
	va_end(args);
	*(str+size-1)='\0';
	return rc;
}

int vsnprintf_mingw32kludge(char *str, size_t size, const  char  *fmt, va_list ap)
{
	int rc;
	rc=_vsnprintf(str,size-1,fmt,ap);
	*(str+size-1)='\0';
	return rc;
}


struct passwd *getpwuid(int id)
{
	static CHAR name[UNLEN + 1]="Nobody";
	DWORD width=UNLEN + 1;
	int i;

	GetUserName(name,&width);
	for (i=0;i<50 && name[i]!='\0';i++)
	{
		if (name[i]==' ')
		{
			name[i]='\0';
			break;
		}
	}
	
	pwdwin32_standard.pw_name = name;

	return &pwdwin32_standard;
}

#endif /* __MINGW32__ */
