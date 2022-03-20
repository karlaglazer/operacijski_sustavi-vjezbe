#include<iostream>
#include<sys/types.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/wait.h>
#include<semaphore.h>
#include<signal.h>
#include<sys/ipc.h>
#include<ctime>
#include<cmath>
#include<cstdlib>
#include<stdio.h>
using namespace std;
#define n 8

int broj;

struct d_mem{
    sem_t semaf[n];
}*p;
key_t dm=shmget(IPC_PRIVATE, sizeof(d_mem), 0600);

void postavljanje(){
    for(int i=0; i<n;i++)
        sem_init(&p->semaf[i], 1, 0);
}

void gotovo(int signal){
    sem_wait(&p->semaf[7]);
    for (int i=0; i<8; i++)
        sem_destroy(&p->semaf[i]);
    shmdt (p);
    shmctl(dm, IPC_RMID, NULL);
    exit(0);
}

int generiraj(){
    srand(time(NULL));
    int sekunde=rand()%9+1;
    return sekunde;
}
void prvi(){
    broj=generiraj();
    for(int i=0; i<broj; i++){
        printf("Izvodim zadatak 1: %i/%i\n", i+1, broj);
        sleep(1);
    }
    sem_post(&p->semaf[0]);

}
void drugi(){
    broj=generiraj();
    for(int i=0; i<broj; i++){
        printf("Izvodim zadatak 2: %i/%i\n", i+1, broj);
        sleep(1);
    }
    sem_post(&p->semaf[2]);
    sem_post(&p->semaf[3]);

}
void treci(){
    broj=generiraj();
    sem_wait(&p->semaf[2]);
    for(int i=0; i<broj; i++){
        printf("Izvodim zadatak 3: %i/%i\n", i+1, broj);
        sleep(1);
    }
    sem_post(&p->semaf[4]);

}
void cetvrti(){
    broj=generiraj();
    sem_wait(&p->semaf[3]);
    for(int i=0; i<broj; i++){
        printf("Izvodim zadatak 4: %i/%i\n", i+1, broj);
        sleep(1);
    }
    sem_post(&p->semaf[6]);

}
void peti(){
    broj=generiraj();
    sem_wait(&p->semaf[0]);
    for(int i=0; i<broj; i++){
        printf("Izvodim zadatak 5: %i/%i\n", i+1, broj);
        sleep(1);
    }
    sem_post(&p->semaf[1]);

}
void sesti(){
    broj=generiraj();
    sem_wait(&p->semaf[4]);
    for(int i=0; i<broj; i++){
        printf("Izvodim zadatak 6: %i/%i\n", i+1, broj);
        sleep(1);
    }
    sem_post(&p->semaf[5]);

}
void sedmi(){
    broj=generiraj();
    sem_wait(&p->semaf[1]);
    sem_wait(&p->semaf[6]);
    sem_wait(&p->semaf[5]);
    for(int i=0; i<broj; i++){
        printf("Izvodim zadatak 7: %i/%i\n", i+1, broj);
        sleep(1);
    }
    sem_post(&p->semaf[7]);
}

int main(){
    system("clear");
    p=(d_mem*)shmat(dm,NULL,0);
    postavljanje();
    sigset(SIGINT, gotovo);
    for(int i=0;i<n;i++){
        pid_t pid=fork();
        if(pid==0){
            if(i==0)
                prvi();
            if(i==1)
                drugi();
            if(i==2)
                treci();
            if(i==3)
                cetvrti();
            if(i==4)
                peti();
            if(i==5)
                sesti();
            if(i==6)
                sedmi();
            exit(0);
        }
        sleep(1);

    }
    gotovo(0);

}