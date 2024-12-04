/*
* IceBreaker
* Copyright (c) 2000-2020 Matthew Miller <mattdm@mattdm.org> 
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

#ifndef LEVEL_H
#define LEVEL_H

#include "line.h"
#include "penguin.h"

typedef enum { ERROR, PASS, DEAD, ZERO, QUIT } LevelExitType;

typedef struct
{
	int basescore;
	int clearbonus;
	int lifebonus;
} ScoreSheet;


extern LevelExitType playlevel(int level, long oldscore, ScoreSheet * score);

extern void redrawwholelevel(void);
extern int checkiflevelatstart(void);

// FIX -- these shouldn't need to be globals
extern Line line1;
extern Line line2;
extern Penguin pg_flock[MAXPENGUINS];
extern int penguincount;


#endif /* LEVEL_H */
