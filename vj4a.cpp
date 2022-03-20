#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<semaphore.h>
#include<signal.h>
#include<sys/ipc.h>
#include<ctime>
#include<cstdlib>
#define dozvola 0600


using namespace std;
pthread_mutexattr_t mut;
pthread_condattr_t con;

struct memorija
{
    sem_t semafor;
    pthread_mutex_t cekanje; 
    pthread_cond_t red;
    int brojac=0;
}*pokazivac;
int zajednicka;

int brprocesa;
int zastavica=0;

void mutexattrpost(){
    pthread_mutexattr_init(&mut);
    pthread_mutexattr_setpshared(&mut, PTHREAD_PROCESS_SHARED);
}

void mutexpost(){
    pthread_mutex_init(&pokazivac->cekanje, &mut);
}

void condattrpost(){
    pthread_condattr_init(&con);
    pthread_condattr_setpshared(&con, PTHREAD_PROCESS_SHARED);
}

void condpost(){
    pthread_cond_init(&pokazivac->red, &con);
}

void sempostavljanje(){
    sem_init(&pokazivac->semafor,1,1);
}

void post(){
    mutexattrpost();
    mutexpost();
    condattrpost();
    condpost();
    sempostavljanje();
}

void gotovo(int signal){
    pthread_mutex_destroy(&pokazivac->cekanje);
    pthread_cond_destroy(&pokazivac->red);
    pthread_mutexattr_destroy(&mut);
    pthread_condattr_destroy(&con);
    sem_destroy(&pokazivac->semafor);
    shmdt (pokazivac);
    shmctl(zajednicka, IPC_RMID, NULL);
    exit(0);
}

void funkcija2(){
    pthread_mutex_lock(&pokazivac->cekanje);
    pokazivac->brojac++;
    if (pokazivac->brojac<brprocesa){
        pthread_cond_wait(&pokazivac->red, &pokazivac->cekanje);
    }
    else {
        pthread_cond_broadcast(&pokazivac->red);
    }
    pthread_mutex_unlock(&pokazivac->cekanje);
}


void funkcija(int brojprocesa){

    sem_wait(&pokazivac->semafor);
    int broj;
    cout<<endl;
    printf("Proces broj %i. unesite broj.",brojprocesa+1);
    cin>>broj;

    sem_post(&pokazivac->semafor);

    funkcija2();
    cout<<endl;
    cout<<"Proces broj "<< brojprocesa+1<< ". uneseni broj: "<<broj<<endl;

}

int main(int argc, char **argv){
    zajednicka=shmget(IPC_PRIVATE, sizeof(memorija), dozvola);
    pokazivac=(memorija*)shmat(zajednicka,NULL,zastavica);
    post();
    brprocesa=atoi(argv[1]);
    cout<<"Broj procesa: "<<brprocesa<<endl;
    for (int i=0;i<brprocesa;i++){
        if (fork()==0){
            funkcija(i);
            exit(0);
        }
    }
    sigset(SIGINT, gotovo);

    for (int i=0; i<brprocesa;i++){
        waitpid(-1,NULL,0);
    }
    gotovo(0);
    return 0;
}