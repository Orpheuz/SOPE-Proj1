#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 

#define MAXLINE 256
///////////////////////////////////////////////////////////////////////////////////////////
//////   Procedimento que verifica de 5 em 5 segundos se os files que o utilizador pediu //
////// para serem monitorizados estao ainda ativos                                       //
///////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv) {
	// [1]<- numero de files
	// [2...]<-nome dos files
	// [2+nFiles]<-pids dos files
	
	int i,nFiles =  strtol(argv[1], NULL, 10);
	char* files[nFiles];
	pid_t pidsAux[nFiles];
	
	
	for(i=0; i<nFiles; i++) // passa todos os nomes de ficheiros e pids dos processos auxiliares que os monitorizam para um array de char's e um de pids
	{
			files[i]=argv[i+2];
			pidsAux[i]=(pid_t)strtol(argv[i+nFiles+2], NULL, 10);
			
	}
		
	while(1){ // enquanto o processo nao for morto, verifica se os ficheiros existem. senao existirem mata os processos auxiliares respetivos
	
	int  killed=0;
		for(i=0; i < nFiles; i++)
	   {
		   
			if (fopen(files[i], "r") == NULL) {
				kill(-pidsAux[i], SIGUSR1);   
				killed++;
				
          }
	  }
	  if(killed==nFiles) // se todos os ficheiros tiverem sido apagados, o monitor(processo pai do file monitor) e todos os seus filhos morrem
		kill(-getppid(), SIGUSR1);
	  sleep(5);
  }
	  exit(0);
}
