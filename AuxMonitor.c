#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <time.h>

#define READ 0
#define WRITE 1
#define MAXLINE 256

//////////////////////////////////////////////////////////////////////////////////////////////////
//////   Procedimento que trata de lançar o tail e o grep que irao monitorizar um dado ficheiro //
//////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	int fd[2], fd1[2];
	pid_t pid_grep, pid_tail;
	
	if (pipe(fd) < 0 || pipe(fd1) < 0) {
		fprintf(stderr, "pipe error\n");
		exit(3);
	}
	
	setpgrp();

	if ( (pid_tail = fork()) < 0 ) {
		fprintf(stderr, "Tail fork error\n");
		exit(4);
	}
	else if (pid_tail > 0) {
		if ( (pid_grep = fork()) < 0 ) {
			fprintf(stderr, "Grep fork error\n");
			exit(5);
		}
		else if (pid_grep > 0) {
			close(fd[WRITE]);
			
			int n;
			do {
				char line[MAXLINE];
				char buffer[252];
				time_t curtime;
				struct tm *loctime;
				n = read(fd[READ], line, MAXLINE);
				line[n-1] = 0;				
				
				curtime = time (NULL);
	
				loctime = localtime (&curtime);
	
				strftime (buffer, 52, "%Y-%m-%dT%H:%M:%S", loctime);
				fputs (buffer, stdout);
				printf(" - %s - \"%s\"\n", argv[1], line);
			} while(n > 0);
		}
		else {
			close(fd1[WRITE]);
			dup2(fd1[READ], STDIN_FILENO);
			dup2(fd[WRITE], STDOUT_FILENO);
			close(fd[READ]);
			
			execlp("grep", "grep", "--line-buffered", argv[2], NULL);
		}
	}
	else {
		close(fd1[READ]);
		dup2(fd1[WRITE], STDOUT_FILENO);
		execlp("tail", "tail", "-f", "-n", "0", argv[1], NULL);
	}
	
	exit(0);
}

