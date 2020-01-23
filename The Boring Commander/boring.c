
/*
 *	boring.c
 *	Authors: Luke Trujillo, Mike Capobianco
 */

#include <stdio.h>
#include <stdlib.h>

#include "boring.h"

int main(int argc, char** argv) {
	execute("whoami");
	execute("last");
	execute("ls -al home");

	return 0;
}

void execute(char* command) {
	//execute the command and collect information

	show(0, 0, 0); //call show with the correct info
}

void show(long elapsedTime, int pageFaults, int reclaimedPageFaults) {
	//display the statistics
	printf("\n-- Statistics ---\n");
	printf("Elapsed time: %ld milliseconds\n", elapsedTime);
	printf("Page Faults: %d\n", pageFaults);
	printf("Page Faults (reclaimed): %d\n", reclaimedPageFaults);
	printf("-- End of Statistics --\n\n");
}



