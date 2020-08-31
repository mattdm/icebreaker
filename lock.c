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


#ifndef WIN32
  #include <fcntl.h>
  #include <unistd.h>
  #include <string.h>
#endif

#include "lock.h"

// the return value of lock/unlock system calls
static int sysret;

// the number of times to retry before returning from lock()
#define RETRY_N 3

// the number of seconds between two lock syscalls (in seconds)
#define INTERVAL 1

//***************** OH MY  *******************************************
// And now for a bunch of crazy cross-platform macros devised
// by Enrico. :)


//***************** LOCK_DEBUG MACRO *********************************
//#include "stdio.h"
//#define LOCK_DEBUG(a...) fprintf(stderr,a)
#define LOCK_DEBUG(a...)

//***************** SYSCALL WRAPPING *********************************
#ifdef WIN32

# define DELAY          	(INTERVAL * 1000)
# define LOCK_SYSCALL(a)   	{sysret=LockFile(a,0,0,1,0);}
# define UNLOCK_SYSCALL(a) 	{sysret=UnlockFile(a,0,0,1,0);}
# define OPEN_SYSCALL(a)	CreateFile(a,GENERIC_READ,1,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,NULL)
# define CLOSE_SYSCALL(a)	CloseHandle(a)
# define SLEEP_SYSCALL()	Sleep(DELAY)
# define BAD_VALUE 		0
# define IS_OK(a)		(a != BAD_VALUE)

#else

static struct flock lockstr;	// used by fcntl
# define DELAY          	INTERVAL
# define LOCK_SYSCALL(a)   	{memset(&lockstr,0,sizeof(lockstr));lockstr.l_type = F_WRLCK;sysret = fcntl(a, F_SETLK , &lockstr);}
# define UNLOCK_SYSCALL(a)      {memset(&lockstr,0,sizeof(lockstr));lockstr.l_type = F_UNLCK;sysret = fcntl(a, F_SETLK , &lockstr);}
# define OPEN_SYSCALL(a)	open(a,O_RDWR)
# define CLOSE_SYSCALL(a)	close(a)
# define SLEEP_SYSCALL()	sleep(DELAY)
# define BAD_VALUE 		-1
# define IS_OK(a)		(a != BAD_VALUE)

#endif

//************************ REAL FUNCTIONS ****************************

/*  return values:
 *     0  = success
 *     -1 = lock failed
 */
int lock(FILE_DESC f)
{
	int i;
	sysret = BAD_VALUE;
	
	for( i = 0 ; i < RETRY_N  && !IS_OK(sysret) ; i++ )
	{
		LOCK_SYSCALL(f);
		
		if(!IS_OK(sysret) && i < RETRY_N -1)
		{
			SLEEP_SYSCALL();	
		}
	}

	if(IS_OK(sysret))
	{
		LOCK_DEBUG("lock obtained\n");
		return 0;
	}
	else
	{
		LOCK_DEBUG("lock denied\n");
		return -1;
	}
}

/*  return values:
 *     0  = success
 *     -1 = unlock failed
 */
int unlock(FILE_DESC f)
{

	UNLOCK_SYSCALL(f);

	if(!IS_OK(sysret))
	{
		LOCK_DEBUG("unlock failed\n");
		// FIX: do something to warn about this, because it shouldn't
		// happen and will definitely cause problems if it does.
		return -1;
	}
	else
	{
		LOCK_DEBUG("unlocked\n");
		return 0;
	}

}

/*  return values:
 *     ?                 = success
 *     INVALID_FILE_DESC = failed
 */
FILE_DESC openlockfile(char* fname)
{
	return OPEN_SYSCALL(fname);
}

void closelockfile(FILE_DESC f)
{
	CLOSE_SYSCALL(f);
}

