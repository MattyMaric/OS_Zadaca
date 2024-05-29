#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
int *PRAVO, *ZASTAVICA; //pokazivaci na dijeljenu memoriju koja ce se koristiti za sinkronizaciju
int pravoId, zastavicaId;	//identifikatori za dijeljenu memoriju

void udi_u_kriticni_odsjecak(int i, int j){
	ZASTAVICA[i] = 1;					// Postavljanjem zastavice na 1 dajemo do znanja da proces "i" zeli uci u kriticni odsjecak
	while(!(ZASTAVICA[j] == 0)){		//Provjeravamo da li je proces j u kriticnom odsjecku, ako je onda on ima prednost da zavrsi svoj proces
		if(*PRAVO  == j){
			ZASTAVICA[i] = 0;			//Ako proces "j" ima prednost onda vracamo zastavicu od i na 0 i ona ostaje da ceka svoj red
			while(*PRAVO == j){
			}							//while loop je ostavljen prazan iz razloga da program moze kontinuirano citati kad ce se proces "j" zavrsiti da mozemo preci na proces "i"
		ZASTAVICA[i] = 1;				//Po zavrsetku postavljamo zastavicu "i" nazad na 1 i taj dio programa moze uci u kriticni odsjecak
		}
	}
}

void izadji_iz_kriticnog_odsjecka(int i, int j){

	ZASTAVICA[i] = 0;
	*PRAVO = j;				
}

void proc(int i, int j){
	int k, m;
	for(k = 1; k<5; k++){
		udi_u_kriticni_odsjecak(i,j);
		for(m = 1; m<5; m++){
			printf("i: %d, k: %d, m: %d\n", i, k, m);
			sleep(1);
		}
		izadji_iz_kriticnog_odsjecka(i,j);
	}
}

void brisi(){
	shmdt(PRAVO);
	shmdt(ZASTAVICA);

	shmctl(pravoId, IPC_RMID, NULL);
	shmctl(zastavicaId, IPC_RMID, NULL);

	exit(0);
}
void main(){
        pravoId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0660);			//Alocira dijeljenu memoriju za pravoID
        zastavicaId = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0660);	//Alocira dijeljenu memoriju za zastavicaId
        PRAVO = (int*)shmat(pravoId, NULL, 0);									//Povezuje PRAVO sa PravoID
        ZASTAVICA = (int*)shmat(zastavicaId, NULL, 0);


        if(fork() == 0){
                proc(1,0);	//Kod se loopa konstantno, samo se mijenjaju vrijednosti varijabli iz 1,0 u 0,1
        }if(fork() == 0){

		proc(0,1);

	};
	signal(SIGINT, brisi);
	wait(NULL);
        wait(NULL);
        brisi();
}
