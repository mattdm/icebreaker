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

#ifndef HISCORE_H
#define HISCORE_H

extern void inithiscores(void);
extern void readhiscores(void);
extern int  checkhiscore(long thisgamescore);
extern int  addhiscore(char* username, long finalscore, int candelay);
extern void writedelayedhiscores(void);

#define HISCORENUM 10


extern char hiscorename[HISCORENUM][50];
extern long hiscoreval[HISCORENUM];

#endif /* HISCORE_H */
