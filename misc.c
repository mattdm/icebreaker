/*
* IceBreaker
* Copyright (c) 2002-2020 Matthew Miller <mattdm@mattdm.org>
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

#ifdef NEEDCHANGETOARGV0PATH
  #include <sys/stat.h>
  #include <unistd.h>
  #include <string.h>
#endif


#ifdef NEEDCHANGETOARGV0PATH
void changetoargv0path(char* argv0)
{
	int chari;
	struct stat st;
	char argv0path[256]; // fix -- should use OS-specific define
	
	if (strlen(argv0)>256)
	{
		fprintf(stderr,"Hmmm -- the executable path seems to be longer than pathnames are allowed\n"
		               "to be. That should never ever happen -- please report this bug.\n");
		return;
	}

	for (chari=strlen(argv0); chari>0 && argv0[chari]!='/'; chari--);
	
	if (chari==0 && (!strcmp(".",homedir) || !strcmp(".",HISCOREPREFIX) || !strcmp(".",DATAPREFIX)))
	{
		// check if argv0 is in current directory, in case . is in the path
		if (stat(argv0,&st) || (!stat(argv0,&st) && !S_ISREG(st.st_mode)))
		{
			fprintf(stderr,"Error: you are running me on a platform where I need to look in the current\n"
			        "directory for some of my files, but I can't find the right place to look --\n"
			        "possibly because %s is in your PATH and you just typed the command\n"
			        "name without any path information. Everything is probably going to break.\n\n"
			        "If you would like to write a smarter function (perhaps one that searches\n"
			        "your path for the executable) for your operating system, please contact me.\n"
			        "See <http://www.mattdm.org/icebreaker/development.html> for more info.\n\n"
			       "Meanwhile, I'm going to try to run, but probably not successfully....\n",argv0);
			return;		        
		}
	}
	else	
	{
		snprintf(argv0path, chari+1, argv0);
		chdir(argv0path);
	}
}
#endif /* NEEDCHANGETOARGV0PATH */
