
/*
 *	boring.c
 *	Authors: Luke Trujillo, Mike Capobianco
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include <sys/wait.h>

#include "boring.h"

int main(int argc, char** argv) {

	char *command1[] = {"whoami", NULL};
	execute(command1);

	char *command2[] = {"last", NULL};
	execute(command2);


	char *command3[] = {"ls", "-al", "/home", NULL};
	execute(command3);

	

	return 0;
}

void execute(char** command) {
	printf("Running command: %s\n", command[0]);


	pid_t child = fork();

	if(child < 0) {
		//there is an error
	} else if(child == 0) { //this is the child process and now call exec
		

		execvp(command[0], command);

	} else { //this is the parent function

		struct timeval start, end;


		gettimeofday(&start, NULL);
		waitpid(child, NULL, 0);
		gettimeofday(&end, NULL);

		struct rusage stats;

		getrusage(RUSAGE_CHILDREN, &stats);

		show((end.tv_sec - start.tv_sec), stats.ru_majflt, stats.ru_minflt);
	}

}

void show(long elapsedTime, int pageFaults, int reclaimedPageFaults) {
	//display the statistics
	printf("\n-- Statistics ---\n");
	printf("Elapsed time: %ld milliseconds\n", elapsedTime);
	printf("Page Faults: %d\n", pageFaults);
	printf("Page Faults (reclaimed): %d\n", reclaimedPageFaults);
	printf("-- End of Statistics --\n\n");
}



