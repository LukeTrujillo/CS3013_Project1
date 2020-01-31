
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
int execute(char*, char**, int, int);
void show(long, long, long);




char workingDirectory[100];


int runningProcess = 0;

int lineNumber;
char line[1000];

struct BackgroundJob {
	int id, running; //running is flag (0 if running)
	char command[100];
};

int jobIndex;

struct BackgroundJob jobs[10];


int main(int argc, char** argv) {
	jobIndex = 0;

	FILE *fp = fopen("multi.txt", "r");

	if(fp != NULL) { //file was opened successfully


	
		lineNumber = 1;
		char fullLine[100];
		//read lines until eof or blank line
		while(readLine(line, 1000, fp)!= NULL && !feof(fp) && strcmp("\n", line)) {
			strcpy(fullLine, line);
			char *command[100];
	
			char *word = strtok(line, " ");
	
			int x = 0;
			//split command at spaces
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


			//check if command is ccd
			if(strcmp(command[0], "ccd") == 0) {

				strcpy(workingDirectory, command[1]);
				printf("Changed to directory: %s\n", workingDirectory);
				
				//check if command is cpwd
			} else if(strcmp(command[0], "cpwd") == 0) {
				chdir(workingDirectory);
	
				char path[100];
				printf("Current directory: %s\n", getcwd(path, 100));

			//check if command is cproclist
			} else if(strcmp(command[0], "cproclist") == 0) {
				
				//print job if its running flag is 0
				for(int b = 0; b < jobIndex; b++) {
					if(jobs[b].running == 0) {
						printf("[%d] %s\n", jobs[b].id, jobs[b].command);
					}
				}
				

			} else {//else execute normally


				int runBG = 0;
				//check if command executes in fore/background
				for(int i = 1; i < argc; i++) {

					if(atoi(argv[i]) == lineNumber) {
						runBG = 1;
						break;
					}
				}
			
					int res = execute(fullLine, command, x, runBG);

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
long bgpageFaults = 0, bgreclaimedPageFaults = 0;

int execute(char* line, char** command, int args, int background) {

	if(background == 0) {//run in foreground
		int status;
		pid_t child = fork();

		
		struct timeval start, end;
		gettimeofday(&start, NULL);


		if(child == 0) {

			chdir(workingDirectory);
			execvp(command[0], command);
			exit(0);
		} else {
			waitpid(child, NULL, 0);
			
			gettimeofday(&end, NULL);

			struct rusage stats;
			getrusage(RUSAGE_SELF, &stats);

			//display statistics
			show(((end.tv_sec - start.tv_sec) * 1000.0) + (end.tv_usec - start.tv_usec) / 1000.0, stats.ru_majflt - pageFaults, stats.ru_minflt - reclaimedPageFaults);

			pageFaults = stats.ru_majflt;
			reclaimedPageFaults = stats.ru_minflt;

		} 
	} if(background == 1) {//run in background
		//add job to array of background jobs
		struct BackgroundJob job;
		job.id = jobIndex;
		job.running = 0;
		strcpy(job.command, line);
	

		jobs[jobIndex] = job;

		printf("Background: ID [%d]: %s\n\n", jobIndex, line);
		

		
		int status;
		pid_t child = fork();

		if(child == 0) {

			pid_t second = fork();
			
			struct timeval start, end;
			gettimeofday(&start, NULL);
		
			if(second == 0) {

			chdir(workingDirectory);
				execvp(command[0], command);
			} else {
		
				waitpid(second, NULL, 0);
				//when jo finishes, say it completed and set running to 1
				printf("\n-- Job Complete [%i: %s] --\n", jobIndex, line);
				printf("Process ID: %d\n", second);
				jobs[jobIndex].running = 1;

				gettimeofday(&end, NULL);

				struct rusage stats;
				getrusage(RUSAGE_SELF, &stats);

				//display statistics
				show(((end.tv_sec - start.tv_sec) * 1000.0) + (end.tv_usec - start.tv_usec) / 1000.0, stats.ru_majflt - bgpageFaults, stats.ru_minflt - bgreclaimedPageFaults);

				bgpageFaults = stats.ru_majflt;
				bgreclaimedPageFaults = stats.ru_minflt;

			}

			exit(0);
			
		} else {
			//increase the number of running jobs
			jobIndex++;
		}


	}
}

void show(long elapsedTime, long pageFaults, long reclaimedPageFaults) {
	//display the statistics
	printf("\n-- Statistics ---\n");
	printf("Elapsed time: %ld milliseconds\n", elapsedTime);
	printf("Page Faults: %ld\n", pageFaults);
	printf("Page Faults (reclaimed): %ld\n", reclaimedPageFaults);
	printf("-- End of Statistics --\n");
}




