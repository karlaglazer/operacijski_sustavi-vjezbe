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
#include<wait.h>
#include<cstdlib>


using namespace std;


pthread_mutexattr_t mon;
pthread_mutexattr_t monn;
pthread_condattr_t vel;
pthread_condattr_t nak;
struct memorija{
    int sifra;
    pthread_mutex_t monitor;
    pthread_mutex_t monitor1;
    pthread_cond_t cekanje;
    pthread_cond_t nakupci;
}*pokazivac;
int zajednicka;
int sig;

void prekid(int sig){

    pthread_mutexattr_destroy(&mon);
    pthread_mutexattr_destroy(&monn);
    pthread_condattr_destroy(&vel);
    pthread_condattr_destroy(&nak);
    pthread_mutex_destroy(&pokazivac->monitor);
    pthread_mutex_destroy(&pokazivac->monitor1);
    pthread_cond_destroy(&pokazivac->cekanje);
    pthread_cond_destroy(&pokazivac->nakupci);
    shmdt ((memorija *)pokazivac);
    shmctl(zajednicka, IPC_RMID, NULL);
    exit(sig);
}


void n1(){
    while(1){
    pthread_mutex_lock(&pokazivac->monitor1);
    if (pokazivac->sifra==1){
        cout<<"Nakupac s racunalima je uzeo monitor i tipkovnicu."<<endl;
        sleep(1);
        pthread_cond_broadcast(&pokazivac->cekanje);
        pthread_cond_wait(&pokazivac->nakupci,&pokazivac->monitor1);
    }
    else{
        pthread_cond_wait(&pokazivac->nakupci,&pokazivac->monitor1);
    }
    pthread_mutex_unlock(&pokazivac->monitor1);
    }
    
}
void n2(){
    while(1){
    pthread_mutex_lock(&pokazivac->monitor1);
    if(pokazivac->sifra==2){
    cout<<"Nakupac s tipkovnicama je uzeo monitor i racunalo."<<endl;
    sleep(1);
    pthread_cond_broadcast(&pokazivac->cekanje);
    pthread_cond_wait(&pokazivac->nakupci,&pokazivac->monitor1);
    }
    else{
        pthread_cond_wait(&pokazivac->nakupci,&pokazivac->monitor1);
    }
    pthread_mutex_unlock(&pokazivac->monitor1);
        }
    
}
void n3(){
    while(1){
    pthread_mutex_lock(&pokazivac->monitor1);
    if(pokazivac->sifra==3){
    cout<<"Nakupac s monitorima je uzeo racunalo i tipkovnicu."<<endl;
    sleep(1);

    pthread_cond_broadcast(&pokazivac->cekanje);
    pthread_cond_wait(&pokazivac->nakupci,&pokazivac->monitor1);
    
    }
    else{
        pthread_cond_wait(&pokazivac->nakupci,&pokazivac->monitor1);

    }
    pthread_mutex_unlock(&pokazivac->monitor1);
    }

}


void vt(){
    int i =0;
    while(i<10){
        pthread_mutex_lock(&pokazivac->monitor);
        int randbroj=rand()%3+1;
        if (randbroj==1){
            cout<<"Veletrgovac je stavio monitor i tipkovnicu."<<endl;
            pokazivac->sifra=1;
            sleep(1);
                   pthread_cond_broadcast(&pokazivac->nakupci);         
        }
        if (randbroj==2){
            cout<<"Veletrgovac je stavio monitor i racunalo."<<endl;
            pokazivac->sifra=2;
            sleep(1);
        pthread_cond_broadcast(&pokazivac->nakupci);

        }    
        else if(randbroj==3){
            cout<<"Veletrgovac je stavio racunalo i tipkovnicu."<<endl;
            pokazivac->sifra=3;
            sleep(1);
                    pthread_cond_broadcast(&pokazivac->nakupci);

        }
        pthread_cond_wait(&pokazivac->cekanje,&pokazivac->monitor);
        pthread_mutex_unlock(&pokazivac->monitor);
        i++;
    }
}



int main(){
    signal (SIGINT, prekid);
    zajednicka=shmget(IPC_PRIVATE, sizeof(memorija), 0600);
    pokazivac=(memorija*)shmat(zajednicka,NULL,0);
    pthread_mutexattr_init(&mon);
    pthread_mutexattr_setpshared(&mon, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&pokazivac->monitor,&mon);
    pthread_mutexattr_init(&monn);
    pthread_mutexattr_setpshared(&monn, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&pokazivac->monitor1,&monn);
    pthread_condattr_init(&vel);
    pthread_condattr_setpshared(&vel, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&pokazivac->cekanje,&vel);
    pthread_condattr_init(&nak);
    pthread_condattr_setpshared(&nak, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&pokazivac->nakupci,&nak);

    cout<<endl;
    for (int i=0;i<4;i++){
        if (fork()==0){
 
            if (i==0)
                vt();
            if(i==1)
                n1();
            if(i==2)
                n2();
            if(i==3)
                n3();
            exit(0);
        }
    }
    for (int i=0; i<4;i++){
        waitpid(-1,NULL,1);
    }

    return 0;
}