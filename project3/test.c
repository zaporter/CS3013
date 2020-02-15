#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
int waiting[200];
int queue[200];
int rooms[10];

int avgPCostumeTime=0;
int avgNCostumeTime=0;
int numP=10;
int numN=2;
int etime=0;

void run(int id){
    while (1){
        while(waiting[id]){}
        printf("HEllo %d\n",id);
        waiting[id]=1;
    }
}
int main(int argc, char** argv){
    printf("Started\n");
    for (int i=0; i<10; i++){printf("%d, %d\n",i,waiting[i]);}    
pthread_t pirate;
    for (int i=0; i<numP; i++){
        pthread_create(&pirate, NULL,run, (void *)i);
    }
    for (int i=0; i<numN; i++){
        pthread_create(&pirate, NULL,run, (void *)i+100);
    }
    while (1){
        int count=0;
        for(int i=0; i<200; i++){count+=waiting[i];}
        if (count!=numP+numN) continue;
        printf("Time: %d\n",etime++);
        for (int i=0; i<200; i++){if(i<numP || (i>=100 && i-100<numN)) waiting[i]=0;}
        
    }
    while (etime++<60*60*24){
        sleep(1000);
    }
    printf("Ended\n");
}
