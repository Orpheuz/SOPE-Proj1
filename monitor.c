#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h>

#define MAXLINE 1000

//////////////////////////////////////////////////////////////////////////////////////////////
//////   Procedimento pai que lança tanto o aux como o file monitor. Tem tambem a função de //
////// de matar todos os processos filhos quando o tempo determinado pelo utilizador acabar //
//////////////////////////////////////////////////////////////////////////////////////////////

int nFiles, timer;
pid_t *pidsAuxM;
pid_t pidFileM;

void usr1_handler(int sig)
{
	if(sig==SIGUSR1)
		exit(0);
}

char* getAuxMPath() { // procedimento que retorna o path onde o aux monitor está localizado 
	char cwd[MAXLINE];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		perror("getcwd() error");
		
	char* AuxMonitorPath;
	AuxMonitorPath = malloc(strlen(cwd)+1);
	strcat(AuxMonitorPath, cwd);
	strcat(AuxMonitorPath, "/AuxMonitor");
	
	return AuxMonitorPath;
}

char* getFileMPath() {// procedimento que retorna o path onde o file monitor está localizado 
	char cwd[MAXLINE];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		perror("getcwd() error");
		
	char* FileMonitorPath;
	FileMonitorPath = malloc(strlen(cwd)+1);
	strcat(FileMonitorPath, cwd);
	strcat(FileMonitorPath, "/FileMonitor");

	return FileMonitorPath;
}

void signalHandler(int signum) { // procedimento encarregue de receber o sinal e matar todos os processos assim que o tempo acabar ou o programa termine
	
	int i;
	for (i = 0; i < nFiles; i++)
		kill(-pidsAuxM[i], SIGINT);	
	kill(-getpid(), SIGINT);
	printf("\nAll processes killed.\n");
	exit(0);
}

int main(int argc, char *argv[]) {
    //argv[0]->nome do proc *
    //argv[1]->tempo *
    //argv[2]->palavra *
    //argv[3]->nome do file *
      
    timer = strtol(argv[1], NULL, 10);
       
    nFiles = argc - 3;
    
    char files[nFiles][MAXLINE];
    
    struct sigaction sigact;
  //  struct sigaction sigzomb;
    
    pid_t pidssAuxM[nFiles];
    pidsAuxM = pidssAuxM;
    
    int i,j;
    for(i = 0; i < nFiles; i++) {
       strcpy(files[i], argv[i+3]);
	}
	
	
    
    
    if(argc < 4) {
		fprintf(stderr, "argc error\n");
		exit(1);	
	}
	
		
	
	
	
	for(i = 0; i < nFiles; i++) {
		if ((pidssAuxM[i] = fork()) < 0 ) {
			fprintf(stderr, "fork error\n");
			exit(3);
		} else if(pidssAuxM[i] > 0) {
			//do nothing
		} else {
			execlp(getAuxMPath(), "AuxMonitor", files[i], argv[2], NULL);
		}
	}
	
	if ((pidFileM = fork()) < 0 ) {
		fprintf(stderr, "fork error\n");
		exit(3);
	}
	else if(pidFileM > 0) {
			//do nothing
			}
	else {
		
		char* line;
		int n=2+2*nFiles; // path do procedimento + numero de files + (nome do file + pid do file)*numero de files
		char* fileMonitorArgv[n+1]; // tamanho do argv + null
		
		// [0]<- .\FileMonitor
		// [1]<- numero de files
		// [2...]<-nome dos files
		// [2+nFiles]<-pids dos files
		
		
		fileMonitorArgv[0]=getFileMPath(); // path
		line = (char*) malloc(256*sizeof(char));
		sprintf(line, "%d", nFiles);
		fileMonitorArgv[1] = line; // numero de files
		fileMonitorArgv[n]=0; // null terminador
		for(j=0; j<nFiles; j++)
		{
			
			fileMonitorArgv[j+2]=files[j]; // passa o nome dos files
			line = (char*) malloc(256*sizeof(char));
			sprintf(line, "%d", pidsAuxM[j]);
			fileMonitorArgv[nFiles+2+j]=line; // passa os pids dos files
		}
		
		execv(getFileMPath(),fileMonitorArgv);
	}
	
	signal(SIGINT, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGUSR1, usr1_handler);
	sigact.sa_handler = signalHandler;

	sigact.sa_flags = 0;
	sigemptyset(&sigact.sa_mask);
	sigaction(SIGALRM, &sigact, NULL);
	
	alarm(timer);
	
	while(1);

	exit(0);
} 
