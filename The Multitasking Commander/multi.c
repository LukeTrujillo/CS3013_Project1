
/*
 *	multi.c
 *	Authors: Luke Trujillo, Mike Capobianco
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>

#include <sys/wait.h>

char* readLine(char*, unsigned int, FILE*);
int execute(char**, int, int);
void show(long, long, long);




char workingDirectory[100];


int runningProcess = 0;

int lineNumber;
char line[1000];

struct BackgroundJob {
	int id, running;
	char *command;
};

int jobIndex;

struct BackgroundJob jobs[10];


int main(int argc, char** argv) {
	jobIndex = 0;

	FILE *fp = fopen("multi.txt", "r");

	if(fp != NULL) { //file was opened successfully


	
		lineNumber = 1;
		
		while(readLine(line, 1000, fp)!= NULL && !feof(fp) && strcmp("\n", line)) {
			char *command[100];
			
			char *word = strtok(line, " ");

			int x = 0;

			for(x = 0; x < 100 && word != NULL; x++) {
				command[x] = word;
				word = strtok(NULL, " ");
			}

			command[x] = NULL;

			printf("Running command: %s", command[0]);

			for(int y = 1; y < x; y++) {
				printf(" %s", command[y]);
			}

			printf("\n");



			if(strcmp(command[0], "ccd") == 0) {

				strcpy(workingDirectory, command[1]);
				printf("Changed to directory: %s\n", workingDirectory);
				
				
			} else if(strcmp(command[0], "cpwd") == 0) {
				chdir(workingDirectory);
	
				char path[100];
				printf("Current directory: %s\n", getcwd(path, 100));

			} else if(strcmp(command[0], "cproclist") == 0) {

				printf("here %d\n", jobIndex);

				for(int b = 0; b < jobIndex; b++) {
					//if(jobs[b].running == 0) {
						printf("[%d] %s\n", jobs[b].id, jobs[b].command);
					//}
				}
				

			} else {


				int runBG = 0;

				for(int i = 1; i < argc; i++) {

					if(atoi(argv[i]) == lineNumber) {
						runBG = 1;
						break;
					}
				}
			
					int res = execute(command, x, runBG);

			}
		
			printf("\n");

			lineNumber++; 

		}

		fclose(fp);


	} else {
		printf("custom.txt could not be found or there was an error gain read permissions to the file.\n");
		return 1;
	}

	
	int status;
	pid_t wpid;

	while((wpid = wait(&status)) > 0);

  	return 0;
}

char* readLine(char *line, unsigned int buffer, FILE *fp) {
	char *result = fgets(line, buffer, fp);

	size_t ln = strlen(line) - 1;

	if(strcmp(line, "\n") == 0)
		return NULL;

	if(line[ln] == '\n') 
		line[ln] = '\0';
	
	return result;
} 

long pageFaults = 0, reclaimedPageFaults = 0;

int execute(char** command, int args, int background) {
	printf("running command %s\n", line);
	

	if(background == 0) {
		int status;
		pid_t child = fork();
		if(child == 0) {
			execvp(command[0], command);
			exit(0);
		} else {
			waitpid(child, NULL, 0);
			printf("done with forground\n");

		} 
	} if(background == 1) {
		
		struct BackgroundJob job;
		job.id = jobIndex;
		//strcpy(job.command, &command[0]);
	

		jobs[jobIndex] = job;
		

		
		int status;
		pid_t child = fork();

		if(child == 0) {

			pid_t second = fork();
		
			if(second == 0) {
				execvp(command[0], command);
			} else {
		
				waitpid(second, NULL, 0);

				//[jobIndex].running = 1;
					
				printf("done with background\n");
			}

			exit(0);
			
		} else {
			jobIndex++;
		}


	}
}

void show(long elapsedTime, long pageFaults, long reclaimedPageFaults) {
	//display the statistics
	printf("\n-- Satistics ---\n");
	printf("Elapsed time: %ld milliseconds\n", elapsedTime);
	printf("Page Faults: %ld\n", pageFaults);
	printf("Page Faults (reclaimed): %ld\n", reclaimedPageFaults);
	printf("-- End of Statistics --\n");
}




