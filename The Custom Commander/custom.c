
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
void show(long, long, long);

char workingDirectory[100];

int main(int argc, char** argv) {
	
	//load custom.txt
	char line[1000];

	FILE *fp = fopen("custom.txt", "r");

	if(fp != NULL) { //file was opened successfully

		
		while(fgets(line, 1000, fp) != NULL && !feof(fp) && strcmp("\n", line)) {
			char *command[100];
			
			char *word = strtok(line, " ");

			int x = 0;

			for(x = 0; x < 100 && word != NULL; x++) {
				command[x] = word;
				word = strtok(NULL, " ");
			}

			command[x] = NULL;
			printf("%s %s\n", command[0], command[1]);


			if(strcmp(command[0], "ccd") == 0) {

				strcpy(workingDirectory, command[1]);
				printf("Changed to directroy: %s\n", workingDirectory);
				
				
			} else {
			
				execute(command);
			}
		
		

		}

		fclose(fp);


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

		chdir(workingDirectory);

		char path[100];
		printf("Current directory: %s\n", getcwd(path, 100));
	
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
	printf("Page Faults: %ld\n", pageFaults);
	printf("Page Faults (reclaimed): %ld\n", reclaimedPageFaults);
	printf("-- End of Statistics --\n\n");
}


