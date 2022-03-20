#include<iostream>
#include<unistd.h>
#include<stdio.h>
#include<csignal>
#include<sys/wait.h>
#include<stdlib.h>
#include<sys/types.h>
#include<limits.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define N 2
#define proc 2

using namespace std;

struct zaj_varijable{
    int flag[N];
    int pass[1];
}*p;

void izlaz(int a, int b){
    p->pass[0] = b;
    p->flag[a] = 0;
}

void k_o(int a, int b){
    p->flag[a] = 1;
    while(p->flag[b]!=0){
        if(p->pass[0]==b){
            p->flag[a] = 0;
            while(p->pass[0]==b);
            p->flag[a] = 1;
        }
    }
}

void funk(int i){
    int b;
    if(i == 1) b=0;
    if (i !=1) b = 1;
    for(int k = 0; k < 5; k++){
     k_o(i, b);
        for(int m = 1; m < 6; m++){
            printf("Proces %i. K.O. %i. (%i/5)\n", i+1, k+1, m);
            sleep(1);
        }
        izlaz(i,b);
    }
} 
key_t d_p;

void prek(int signal){
    shmdt (p);
    shmctl(d_p, IPC_RMID, NULL);
    exit(0);
}
int main(){
    sigset(SIGINT, prek);

    if(d_p==-1){
        cout<<"Pogreska!"<<endl;
        exit(1);
    }
    
    d_p= shmget(IPC_PRIVATE, sizeof(zaj_varijable), 0600);
    p = (zaj_varijable*) shmat(d_p, NULL, 0);
    


    for(int a = 0; a<proc; a++){

        pid_t pid=fork();

        if(pid==-1)
        {
            cout<<"Pogreska!"<<endl;
            exit(0);
        }
        if(pid==0){
            funk(a);
            exit(0);
        }
    }
    for(int b = proc; b>=0; b--)
        waitpid(-1, NULL, 0);
    prek(0);
    
    return 0;
}