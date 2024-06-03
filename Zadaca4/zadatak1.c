#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#define N 5		//Broj mjesta
#define M 15	//Broj posjetitelja
#define SEMNAME_MJESTA "mjesta" //Naziv semafora

void posjetitelj(int i){

	sem_t* mjesta = sem_open(SEMNAME_MJESTA, 0);
	while(1){
		sem_wait(mjesta);	//Ceka da se mjesto oslobodi na vrtuljku
		printf("proces %d je usao u vrtuljak\n", i);	
		sleep(3);	//Simulira vrijeme provedeno na vrtuljku
	}	
}

void vrtuljak(){
int br = 0;
	sem_t* mjesta = sem_open(SEMNAME_MJESTA,0);	//Alocira memoriju za mjesta
	int * value = malloc(sizeof(int));
	while(1){
		do{
			sem_getvalue(mjesta, value);	//Ispise koliko ima slobodnih mjesta
			printf("broj slobodnih mjesta: %d\n", *value);
			sleep(1);
		}while(*value != 0);
	
	sleep(1);
	printf("vrtuljak se vrti %d\n", br++);	//Simulira rad vrtuljka
	sleep(3);
	printf("Vrtuljak se zaustavio\n");
	
	for(int i=0;i<N; i++){
	sem_post(mjesta);	//Oslobodi zauzeta mjesta na vrtuljku
	printf("Sjedalo br %d na vrtuljku se ispraznilo!\n", i+1);
	}
	
	}
	
}


void main(){
       
	sem_t* mjesta = sem_open(SEMNAME_MJESTA, O_CREAT, 0644, N);	//Inicijalizacija semafora "mjesta" sa N sjedista
	
	for(int i=0;i<N; i++){
	sem_post(mjesta);		//Ovaj dio osigurava da su sva mjesta dostupna
	}
	
	if(fork() == 0){
		vrtuljak();	//Pokrece child process da pokrene vrtuljak funkciju
	}
	
	sleep(1);
	for(int i = 0; i<M;i++){
		if(fork() == 0){
			posjetitelj(i);	//Pokrene M procesa da zapocnu posjetitelj funkciju, svaki ima ID od 0 do 14
		}					
	}
	

	for(int i = 0; i<M+1; i++){
		wait(NULL); //Program ceka da se procesi zavrse 
	}
	
	sem_destroy(mjesta);		//Unistavanje semafora "mjesta"

}
  
