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

/* 
*  This file contains function prototypes, macros, etc. to aid in win32
*  cross-compiling using mingw32 (based on gcc version 2.95.2 19991024
*  (release))
*
*  This is Enrico Tassi's <gareuselesinge@infinito.it> domain. :)
*/


#ifndef WIN32_COMPATIBILITY_H
#define WIN32_COMPATIBILITY_H

// fix -- make this also work with cygwin

#ifdef __MINGW32__

#include <stdarg.h>

/* mingw32 is missing snprintf. It has  _snprintf, 
   but unfortunately and stupidly, the size parameter has an off-by-one
   error causing it to write one more char than requested. this is a
   workaround.... */
#define snprintf snprintf_mingw32kludge
#define vsnprintf vsnprintf_mingw32kludge
int snprintf_mingw32kludge(char *str, size_t size, const  char  *format, ...);
int vsnprintf_mingw32kludge(char *str, size_t size, const  char  *format, va_list ap);

/* mingw32 has no index() in string.h */
extern char *index(const char *s, int c);

/* mingw32 has 'short' random functions */
#define srandom(A) srand(A)
#define random() rand()


/* No pwd.h in mingw32 */
#define uid_t int
#define gid_t int
	
struct passwd{
              char    *pw_name;       /* user name */
              char    *pw_passwd;     /* user password */
              uid_t   pw_uid;         /* user id */
              gid_t   pw_gid;         /* group id */
              char    *pw_gecos;      /* real name */
              char    *pw_dir;        /* home directory */
              char    *pw_shell;      /* shell program */
      };


/* No pwd.h -> no getpw... */	
#define getuid() 0

extern struct passwd *getpwuid(int id);

#else

#error Please configure win32_compatibility.h / win32_compatibility.c for your compiler and send the results to <mattdm@mattdm.org>. Thank you!

#endif // __MINGW__


#endif /* WIN32_COMPATIBILITY_H */
