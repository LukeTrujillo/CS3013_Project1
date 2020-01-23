
/*
 *	boring.h
 *	Authors: Luke Trujillo, Mike Capobianco
 */

#ifndef BORING_H
#define BORING_H
	//header stuff going here

	void execute(char** command); //executes the given command, calls show() to display statistics

	void show(long elaspedTime, int pageFaults, int reclaimedPageFaults);
#endif
