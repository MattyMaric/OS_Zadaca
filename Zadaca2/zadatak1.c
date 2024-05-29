#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <pthread.h>
#define SHARED_MEMORY_SIZE (0x6400)

int segment_Id; //ID za dijeljenu memoriju
int *sh_mem;	//Pokazivac na dijeljenu memoriju


void brisi(){	//Signal koji salje poruku te prekida i brise segmente dijeljene memorije
	printf("signal primljen gasi sve");
	shmdt(sh_mem);
	shmctl(segment_Id, IPC_RMID, NULL); 
	exit(0);
}

void main(){
	segment_Id = shmget(IPC_PRIVATE, SHARED_MEMORY_SIZE, IPC_CREAT | 0660 );	//0660 - specificira stvaranje i read-write dozvole
	sh_mem = (int*) shmat(segment_Id, NULL, 0);
	while(1){
		if(fork() == 0){
			*sh_mem = 1;
			printf("%d\n", *sh_mem);
			sleep(3);
			printf("%d\n", *sh_mem);
		  	exit(0);					//Ako je fork == 0, postavlja sh_mem na 1, ispise ga ne ekran, te ga poslije 3 sek ispise ponovo i izlazi
		}

			signal(SIGINT, brisi);
			*sh_mem = 2;
			printf("%d\n", *sh_mem);
			sleep(3);
			printf("%d\n", *sh_mem);
			wait(NULL);					//Ovaj dio koda poziva signal za brisanje, postavlja sh_mem na 2, ispisuje vrijednost 2 puta te ceka da se zavrsi proces sa wait(NULL)

	}
}
