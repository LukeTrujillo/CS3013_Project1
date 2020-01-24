
/*
 *	custom.c
 *	Authors: Luke Trujillo, Mike Capobianco
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>

#include <sys/wait.h>

void execute(char**);
void show(

int main(int argc, char** argv) {
	
	//load custom.txt

	char *name = "custom.txt";
	char line[1000];

	FILE *fp = fopen(name, "r");

	if(fp != NULL) { //file was opened successfully

		while(fgets(str, 1000, fp) != NULL) {
			// process each command here -- using strtok
			//handle errors and current directory as well

		}


	} else {
		printf("custom.txt could not be found or there was an error gain read permissions to the file.\n");
		return 1;
	}

	

	return 0;
}

long pageFaults = 0, reclaimedPageFaults = 0;

void execute(char** command) {
	printf("Running command: %s", command[0]);

	if(command[1] != NULL) { 
		printf(" %s", command[1]);
		if(command[2] != NULL) {
			printf(" %s", command[2]);
		}
	}
	
	printf("\n");


	pid_t child = fork();

	struct timeval start, end;
	gettimeofday(&start, NULL);


	if(child < 0) {
		//there is an error
	} else if(child == 0) { //this is the child process and now call exec
		execvp(command[0], command);


	} else { //this is the parent function


		waitpid(child, NULL, 0);
		gettimeofday(&end, NULL);

		struct rusage stats;
		getrusage(RUSAGE_SELF, &stats);


		show(((end.tv_sec - start.tv_sec) * 1000.0) + (end.tv_usec - start.tv_usec) / 1000.0, stats.ru_majflt - pageFaults, stats.ru_minflt - reclaimedPageFaults);

		pageFaults = stats.ru_majflt;
		reclaimedPageFaults = stats.ru_minflt;
	}

}

void show(long elapsedTime, long pageFaults, long reclaimedPageFaults) {
	//display the statistics
	printf("\n-- Statistics ---\n");
	printf("Elapsed time: %ld milliseconds\n", elapsedTime);
	printf("Page Faults: %d\n", pageFaults);
	printf("Page Faults (reclaimed): %d\n", reclaimedPageFaults);
	printf("-- End of Statistics --\n\n");
}


