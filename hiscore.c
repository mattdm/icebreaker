/*
* IceBreaker
* Copyright (c) 2000-2002 Matthew Miller <mattdm@mattdm.org> and 
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

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "icebreaker.h"
#include "globals.h"
#include "hiscore.h"
#include "lock.h"

char hiscorename[HISCORENUM][50];
long hiscoreval[HISCORENUM];

static char temphiscorename[HISCORENUM+1][50]; //used for sorting
static long temphiscoreval[HISCORENUM+1];

// used if we can't get write access to the high score file immediately
static char delayedhiscorename[HISCORENUM+1][50]; 
static long delayedhiscoreval[HISCORENUM+1];

static int cmpscore(int * a, int * b);
static void sorthiscore(char hiname[HISCORENUM][50],long hival[HISCORENUM],char * username, long score);
static void writehiscores(char * username, long thisgamescore);
static void adddelayedhiscore(char* username,long finalscore);

void inithiscores()
{
	int i;
	readhiscores();

	// zero out the just-in-case array
	for (i=0;i<HISCORENUM;i++)
	{
		delayedhiscoreval[i]=0;
	}
}

void readhiscores()
{
	FILE *hiscorefile;
	char linebuf[80];
	char filename[274]; // fix -- use defined OS constant

	int arrayindex[HISCORENUM];
	int i;

	// fill the "helper" array. 
	for (i=0;i<HISCORENUM;i++)
		arrayindex[i]=i;
		

	// make sure all entries are zeroed out to start.
	for (i=0;i<HISCORENUM;i++)
	{
		snprintf(temphiscorename[i],7,"Nobody");
		temphiscoreval[i]=100; //100 is better than 0. :)
	}

	snprintf(filename,274,"%s/%s",homedir,HISCOREFILE);

	hiscorefile=fopen(filename,"r");
	if (hiscorefile==NULL)
	{
		// It's writing we need to worry about, really, so don't
		// complain here.
		//fprintf(stderr,"Can't read high score file; continuing anyway.\nYou may want to ask your sysadmin to make sure this program can access\n " homedir "/" HISCOREFILE "\n");
	}
	else
	{
		for (i=0;i<HISCORENUM;i++)
		{
			if (fgets(linebuf,80,hiscorefile))
			{
				sscanf(linebuf,"%49s %30ld",temphiscorename[i],&temphiscoreval[i]);
			}
		}
		fclose(hiscorefile);
		
		// sort arrayindex based on the corresponding hiscoreval
		// really, the array should already be sorted. but you never know.	
		qsort(arrayindex, HISCORENUM, sizeof(int), (int (*)(const void*,const void*))cmpscore);

	}
	
	// ok, so now, we can copy things over in the proper sorted order
	for (i=0;i<HISCORENUM;i++)
	{
		snprintf(hiscorename[i],50,"%s",temphiscorename[arrayindex[i]]);
		hiscoreval[i]=temphiscoreval[arrayindex[i]];
	}

}


int checkhiscore(long thisgamescore)
{
	// need to re-read from disk in case another user has obtained
	// a better score in the meantime...
	readhiscores();
	// check to see if score is better than the lowest high score
	return (thisgamescore>hiscoreval[HISCORENUM-1]);
}

int addhiscore(char* username, long finalscore, int candelay)
{
	int gotlock=false;
	int rc=-1;
	FILE_DESC filelock;
	char filename[274]; // fix -- use defined OS constant
	
	snprintf(filename,274,"%s/%s",homedir,HISCORELOCKFILE);

	filelock = openlockfile(filename);
	if (filelock == INVALID_FILE_DESC)
	{
		 fprintf(stderr,"Can't access lock file -- this means we can't save the high scores.\n"
	 	                "You may want to ask your sysadmin to make sure this program can write to\n"
		                "<%s>.\n",filename);
	}
	else
	{
		gotlock = lock(filelock);
		if (!gotlock)
		{
			if (checkhiscore(finalscore))
				writehiscores(username,finalscore);
			else
				rc=0;

			gotlock=unlock(filelock);

			if (gotlock)
			{
				fprintf(stderr,"Warning: something very strange has happened. This isn't good.\n"
				               "Can't unlock <%s>!\n",filename);
		        }     
		}
		else if (candelay)
		{
			adddelayedhiscore(username,finalscore);
		}
		else
		{
			fprintf(stderr,"Someone is holding onto the lock file and we can't save high scores, so\n"
			               "despite our best efforts, %s's high score of %ld was dropped.\n",username,finalscore);
		}	

		closelockfile(filelock);
	}

	return rc;

}

void writedelayedhiscores()
{
	int i;

	for (i=0;i<HISCORENUM;i++)
	{
		if(delayedhiscoreval[i]!=0) // fix -- we're done if this happens, right? 
		{
			if(!addhiscore(delayedhiscorename[i],delayedhiscoreval[i],false))
			{
				// I think it's let better to let people figure this
				// out by themselves than to clutter up stderr -- they
				// can just compare and see that the score was beaten.
				//fprintf(stderr,"Sorry: %s %ld was a high score once, but now it's been beaten.\n",delayedhiscorename[i],delayedhiscoreval[i]);
			}
			else
			{
				// pointless at the end of the game, but needed if this function
				// were to be called midgame.
				delayedhiscoreval[i]=0;
			}
		
		}
	}
}

void sorthiscore(char hiname[HISCORENUM][50],long hival[HISCORENUM],char * username, long score)
{
	int arrayindex[HISCORENUM+1]; // note the +1 -- we're including the new score
	int i;

	// make sure the temp array contains the right data
	for (i=0;i<HISCORENUM;i++)
	{
		snprintf(temphiscorename[i],50,"%s",hiname[i]);
		temphiscoreval[i]=hival[i];
	}
	
	// and toss in the new data 
	//(this is why these arrays are size HISCORENUM+1)
	snprintf(temphiscorename[HISCORENUM],50,"%s",username);
	temphiscoreval[HISCORENUM]=score;

	// fill the "helper" array. 
	for (i=0;i<HISCORENUM+1;i++)
		arrayindex[i]=i;

	
	// ok, now sort those 
	qsort(arrayindex, HISCORENUM+1, sizeof(int), (int (*)(const void*,const void*))cmpscore);

	// and take the top ones back.
	for (i=0;i<HISCORENUM;i++)
	{
		snprintf(hiname[i],50,"%s",temphiscorename[arrayindex[i]]); 
		hival[i]=temphiscoreval[arrayindex[i]];
	}
}

void writehiscores(char * username, long thisgamescore)
{
	FILE *hiscorefile;
	char filename[274]; // fix -- use defined OS constant
	int i;

	sorthiscore(hiscorename,hiscoreval,username,thisgamescore);

	snprintf(filename,274,"%s/%s",homedir,HISCOREFILE);
	
	hiscorefile=fopen(filename,"w");
	if (hiscorefile==NULL)
	{
		fprintf(stderr,"Can't save high scores."
		               "You may want to ask your sysadmin to make sure this program can write to\n"
	                       "<%s>.\n",filename);
	}
	else
	{
		// FIX -- make this go so people can have spaces in their names
		//for (i=0;hiscorename[i]!='\0';i++)
		//      if (hiscorename[i]==' ') hiscorename[i]='_';
	
		for (i=0;i<HISCORENUM;i++)
		{
			fprintf(hiscorefile,"%s %ld\n",hiscorename[i],hiscoreval[i]);
		}
			
		fclose(hiscorefile);
	}
}

void adddelayedhiscore(char* username,long finalscore)
{
	// int i;

	sorthiscore(delayedhiscorename,delayedhiscoreval,username,finalscore);

	//Uncomment for debugging
	/*for (i=0;i<HISCORENUM;i++)
	{
		if(delayedhiscoreval[i] != 0)
			printf("%2d : %s %ld\n",i,delayedhiscorename[i],delayedhiscoreval[i]);
	} */             
}

int cmpscore(int * a, int * b)
{
	return(temphiscoreval[*b] - temphiscoreval[*a]);
}


