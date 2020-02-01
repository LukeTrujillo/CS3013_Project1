
/*
 *	boring.h
 *	Authors: Luke Trujillo, Mike Capobianco
 */

#ifndef BORING_H
#define BORING_H
	void execute(char** command); //executes the given command, calls show() to display statistics

	void show(long elaspedTime, long pageFaults, long reclaimedPageFaults);
#endif
