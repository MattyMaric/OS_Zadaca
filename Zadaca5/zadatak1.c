#include<stdio.h>
#include <stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#define N 5


int fil_br[5] = {0,1,2,3,4}; 	//Index filozofa
sem_t sem[N];	// Niz semafora, kontrolira pristup forkovima
sem_t mutex;	//Mutex osigurava da samo jedan filozof moze promijenuti svoje stanje, sto preventira utrke uvjeta
int state[N];	//Niz koji predstavlja stanje filozofa

void test(int br){	//Provjerava da li filozof "br" moze krenut jest
	//1 - gladan, 2 - jede, 3 - misli
	if(state[br] == 1 && state[(br+4)%N] != 2 && state[(br+1)%N] != 2 ){ //Uvjet je da Filozof mora biti gladan i da njemu susjedni filozofi ne jedu
		
		state[br] = 2;
		printf("filozof %d uzima vilice i jede\n", br +1); //Semafor se inkrementira
		fflush(stdin);
		sleep(1);
		sem_post(&sem[br]);
	}
}

void uzima_vilicu(int br){ //Poziva se kad filozof uzima vilicu 	
	sem_wait(&mutex); //Poziva mutex da osigura medusobno iskljucivanje
	state[br] = 1;	
	printf("filozof %d je gladan\n", br +1 );
	test(br); 	//Poziv test funkcije da vidi moze li filozof jesti
	sem_post(&mutex);
	sem_wait(&sem[br]); //Ceka na semafor od filozofa
	fflush(stdin);
	sleep(1);
}

void vrati_vilice(int br){	//Koristi se kad filozof spusti vilice nakon hranjenja
	sem_wait(&mutex);	//Pribavlja mutex
	
	printf("filozof %d ostavlja vilice i ide misliti\n", br +1);
	fflush(stdin);
	sleep(1);
	state[br] = 3;
	test((br+4)%N); //Poziv test funkcije na susjede
	test((br+1)%N);
	sem_post(&mutex);	 //Otpusta mutex
}

void* filozof(void* pdr){ //Glavna funkcija za filozofa
	int temp =*((int *)pdr);
	while(1){
		int temp =*((int *)pdr);
		sleep(2);
		uzima_vilicu(temp);		//Infinite loop gdje filozof se izmjenjuje izmedu misljenja i hranjenja
		sleep(0);
		vrati_vilice(temp);
	}
}

void main(){
	pthread_t thread_id[N];
	sem_init(&mutex, 0, 1);	//Inicijalizacija mutex semafora
	int br;
	for(int i=0; i<N; i++){
		sem_init(&sem[i],0,0);
		state[i] = 3;	//Postavljanje svih filozofa na stanje razmisljanja
	}
	for(int i=0; i<N; i++){
		int *br = (int *) malloc(sizeof(int));	//Alociranje memorije za filozofe
		*br = i;
		pthread_create(&thread_id[i], NULL, filozof,  br); //Pokretanje novog threada gdje je index filozofa argument
		printf("Filozof %d misli\n", i+1);	
	}
	for(int i=0; i<N; i++){
		pthread_join(thread_id[i], NULL);	//Ceka da svi threadovi zavrse, ali se to nikad ne dogodi zbog infinite loopa
	}
	
}
