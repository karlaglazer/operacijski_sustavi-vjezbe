#include<iostream>
#include<cstring>
#include<time.h>
#include<unistd.h>
#include<csignal>
#include<pthread.h>
#include<sstream>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/wait.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
using namespace std;

int bd;
int bb;
pthread_t *pd;
int *polj;

struct spremnik{
    int ulaz;
    int izlaz;
    int ukupno;
    int m[5];
}*pok;
int spr;
int semafor;
int pun=0, pisi=1, prazan=2;


void semafori(){
    semafor=semget(IPC_PRIVATE, 3, 0600);
    semctl(semafor, pun, SETVAL, 5);
    semctl(semafor, pisi, SETVAL, 1);
    semctl(semafor, prazan,  SETVAL, 0);
}

int poziv(int broj, int oper){
    struct sembuf Sembuf;
    Sembuf.sem_num=broj;
    Sembuf.sem_op=oper;
    Sembuf.sem_flg=0;
    return semop(semafor, &Sembuf, 1);
}



void *pro(void *arg){
    int a=*((int*)arg);
    int br;
    for(int i=0; i<bb; i++){
        poziv(pun,-1);
        poziv(pisi,-1);
        pok->m[pok->ulaz]=rand()%1000;
        int br=pok->m[pok->ulaz];
        printf("Proizvodac %i salje %i.\n", a+1, br);
        pok->ulaz=(pok->ulaz+1)%5;
        usleep(100000); 
        poziv(pisi,1);
        poziv(prazan,1);
   
    }
    printf("Proizvodac %i je zavrsio.\n", a+1);
    return NULL;
}


void *pot(void *arg){
    int zbroj=0;
    for(int i=0; i<pok->ukupno;i++){
        poziv(prazan, -1);
        zbroj+=pok->m[pok->izlaz];
        int br=pok->m[pok->izlaz];
        printf("Potrosac prima %i.\n", br);
        pok->izlaz=(pok->izlaz+1)%5;
        usleep(100000);
        poziv(pun, 1);

    }

    printf("Zbroj je %i.\n",zbroj);
    poziv(prazan,1);
    return NULL;
}

void gotovo(int signal){
    poziv(prazan,-1);
    semctl(semafor, 0, IPC_RMID, 0);
    if(!signal){
        for(int i=0;i<bd;i++)
            pthread_join(pd[i],NULL);
    }
    else{
        for(int i=0;i<bd;i++)
            pthread_kill(pd[i],SIGKILL);   
    }
    shmdt((spremnik*)pok);
    shmctl(spr, IPC_RMID, NULL);
    delete []polj;
    delete []pd;
    exit(0);
}



int main(int argc, char **argv){
    system("clear");

    bd=atoi(argv[1]);
    bb=atoi(argv[2]);
    pd=new pthread_t [bd];
    polj=new int [bd];
    pthread_t potrosac;
    spr=shmget(IPC_PRIVATE, sizeof(spremnik), 0600);

    pok=(spremnik*)shmat(spr, NULL, 0);
    pok->ukupno=bb*bd;
    semafori();

    for (int i=0; i<bd; i++){
        polj[i]=i;
        pthread_create(&pd[i], NULL, pro, polj+i);
        usleep(500000);
        if(i==0)
            pthread_create(&potrosac, NULL, pot, NULL);
        usleep(500000);
    }
    sigset(SIGINT, gotovo);
    gotovo(0);
    return 0;
}