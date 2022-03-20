#include<iostream>
#include<cstdlib>
#include<ctime>
#include<pthread.h>
#include<csignal>
#include<stdio.h>

using namespace std;

struct spolje{
    int *p;
    int broj_elemenata;
    int velicina;
}*polje;
int br_dretvi;
pthread_t *p_dretvi;

void *funkcija(void *arg){
    int i=*((int*)arg);
    int pocetak=polje->velicina*i;
    int zavrsetak=pocetak+polje->velicina;

    if(zavrsetak>polje->broj_elemenata)
        zavrsetak=polje->broj_elemenata;

    int zbroj=0;
    int kursor=pocetak;
    while(kursor<zavrsetak)
        zbroj+=polje->p[kursor++];

    printf("Zbroj (%i-%i) = %i\n", pocetak, zavrsetak-1, zbroj);
}

void gotovo(int sig){
    if(!sig){
        for(int i=0;i<br_dretvi;i++)
            pthread_join(p_dretvi[i],NULL);
    }
    else{
        for(int i=0;i<br_dretvi;i++)
            pthread_kill(p_dretvi[i],SIGKILL);   
    }
    exit(0);
}

int main(int argc, char **argv){

    polje=new spolje;
    polje->broj_elemenata=atoi(argv[1]);
    polje->velicina=atoi(argv[2]);
    polje->p=new int[polje->broj_elemenata];


    srand(time(0));
    for(int i=0; i<polje->broj_elemenata;i++){
        polje->p[i]=rand()%10000+1;
        cout<<i<<". "<<polje->p[i]<<endl;
    }

    br_dretvi=polje->broj_elemenata/polje->velicina;

    int *ipolje=new int [br_dretvi];

    if(polje->broj_elemenata%polje->velicina)
        br_dretvi++;
    
    p_dretvi=new pthread_t [br_dretvi];
    cout<<"Broj dretvi: "<<br_dretvi<<endl;
    for (int i=0;i<br_dretvi;i++){
        ipolje[i]=i;
        pthread_create(&p_dretvi[i], NULL, funkcija, ipolje+i);
    }

    gotovo(0);
}