
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
#include <time.h>

#define MAX_JOBS 100
#define CMD_MAX_LENGTH 1000

void check();
void checkBackground();
void checkForeground();

void next();


char line[CMD_MAX_LENGTH];
char *execCMD[100];

FILE *fp;
char* getLine();

int lineNum;
int end_of_file;
int cpy_argc;
char** cpy_argv;

void show(long, long, long);

void getExecCMD();

struct Job {
	pid_t process;
	int id, running, background;
	char command[CMD_MAX_LENGTH];
	struct timeval start, end;
};

struct Job running[MAX_JOBS];
int jobIndex;
int lastJobID;

long pageFaults = 0;
long reclaimedPageFaults = 0;

char workingDirectory[100];

int main(int argc, char** argv) {
	lastJobID = 0;
	jobIndex = 0;
	lineNum = 0;

	cpy_argc = argc;
	cpy_argv = argv;


	//file setup stuff
	fp = fopen("multi.txt", "r");
	
	if(fp != NULL) { //file successfully opened
		end_of_file = 0;

		next();
		
		int status;
//!end_of_file && wait(&status) > 0
		while(1) {
			check();


			if(end_of_file) {	
				int found = 0;
				for(int x = 0; x <= jobIndex; x++) {
					if(running[x].running == 1) {found = 1; break;}
				}

				if(found == 0) {break;}
			}
		}


		check();

		fclose(fp);
	}




 return 0;
}


void check() {	


	for(int x = 0; x < MAX_JOBS; x++) {

		int status;
		if(running[x].running == 1 && waitpid(running[x].process, &status, WNOHANG) == running[x].process) {
			gettimeofday(&running[x].end, NULL);
			running[x].running = 0;


			if(running[x].background) {
				printf("\n-- Job Complete[%d: %s]--\nProcess ID: %d\n\n", running[x].id, running[x].command, running[x].process);
			}
	
			struct rusage stats;
			getrusage(RUSAGE_CHILDREN, &stats);


			long seconds = running[x].end.tv_sec - running[x].start.tv_sec;
			long micros = ((seconds * 1000000) + running[x].end.tv_usec) - (running[x].start.tv_usec);
		

			show((micros / 1000), stats.ru_majflt - pageFaults, stats.ru_minflt - reclaimedPageFaults);

		

			pageFaults = stats.ru_majflt;
			reclaimedPageFaults = stats.ru_minflt;

			if(!end_of_file && !running[x].background) { next(); } //call for the next task to be executed

		}

	}

}



void getExecCMD() {

	char l[CMD_MAX_LENGTH];

	strcpy(l, line);

	char *word = strtok(l, " ");

	int x = 0;

	for(x = 0; x < 100 && word != NULL; x++) {
		execCMD[x] = word;
		word = strtok(NULL, " ");
	}

	
}

//only called by foreground
void next() {

	//read in the file
	char *res = getLine(); // reads next line and stores in line variable

	if(res == NULL || !strcmp("\n", line)) { //file is over so no more reading
		end_of_file = 1;
		return;
	}

	lineNum++;

	
	int background = 0; //determine if its backfground
	
	for(int x = 0; x < cpy_argc; x++) {
		if(lineNum == atoi(cpy_argv[x])) {
			background = 1;
			break;
		}
	}

	getExecCMD(); //format the line for the exec call



	printf("\nRunning command: %s\n", line);

	if(strcmp(execCMD[0], "ccd") == 0) {

		strcpy(workingDirectory, execCMD[1]);
		printf("Changed to directory: %s\n", workingDirectory);

		next();

		return;
		
		//check if command is cpwd
	} else if(strcmp(execCMD[0], "cpwd") == 0) {
		chdir(workingDirectory);

		char path[100];
		printf("Current directory: %s\n", getcwd(path, 100));

		next();
		return;
	//check if command is cproclist
	} else if(strcmp(execCMD[0], "cproclist") == 0) {
		
		//print job if its running flag is 0
		for(int b = 0; b < jobIndex; b++) {


			if(running[b].running == 1 && running[b].background == 1) {
				printf("[%d] %s\n", running[b].id, running[b].command);
			}
		}
		
		next();
		return;
	}


	struct Job job;
	job.running = 1;

	strcpy(job.command, line);

	if(background == 1) {
		job.background = 1;
		job.id = lastJobID;

		printf("Background: ID [%i]: %s\n\n", job.id, job.command);
		lastJobID++;
	} else {
		job.background = 0;
	}

	gettimeofday(&job.start, NULL);

	job.process = fork();

	if(job.process == 0) { //child
		chdir(workingDirectory);
		execvp(execCMD[0], execCMD);
		exit(0); //close the child to avoid bad things
	}

	running[jobIndex] = job;
	jobIndex++;

	if(background) {
		next();
	}

}

char* getLine() {
	char *result = fgets(line, CMD_MAX_LENGTH, fp);

	size_t ln = strlen(line) - 1;

	if(strcmp(line, "\n") == 0)
		return NULL;

	if(line[ln] == '\n') 
		line[ln] = '\0';

	return result;
}


void show(long elapsedTime, long pageFaults, long reclaimedPageFaults) {
	//display the statistics
	printf("\n-- Statistics ---\n");
	printf("Elapsed time: %ld milliseconds\n", elapsedTime);
	printf("Page Faults: %ld\n", pageFaults);
	printf("Page Faults (reclaimed): %ld\n", reclaimedPageFaults);
	printf("-- End of Statistics --\n");
}
