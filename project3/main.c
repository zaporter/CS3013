#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int queue[200];
int rooms[10];
int cost[200];
sem_t roomLock;
int avgPCostumeTime=0;
int avgNCostumeTime=0;
int numP=30;
int numN=30;
int numRooms=4;
int running=0;

int getRoom(int start){
    int minID=-1, minVal=2147483647;// int max
    for (int i=start; i<start+100; i++){
        int notIn=1;
        for (int j=0; j<10; j++){ if (rooms[j]==i) notIn=0;}
        if (queue[i]<minVal && queue[i]!=0 && notIn){
            minVal=queue[i];
            minID=i;
    }}
    return minID;
}
void run(int id){
    sleep(rand()%15);
    int taken=0;
    sem_wait(&roomLock);
    for (int i=0; i<numRooms; i++){
        if (rooms[i]==-1){
            rooms[i]=id;
            break;
        }
    }
    sem_post(&roomLock);
    printf("%d, watin\n",id);
    queue[id]=time(NULL);
    int room=-1;
    while (1){
        for (int i=0; i<10; i++){
            if (rooms[i]==id){
                room=i;
            }
        }
        if (room!=-1){
            sleep(1);
            break;
        }
    }
    printf("%d, IM CHANGIN' IN ROOM %d MATEE\n",id,room);
    int waittime=time(NULL)-queue[id];
    int sleepTime= rand()%5;
    queue[id]=0;
    sleep(sleepTime);
    cost[id]+=waittime>30?0:sleepTime;
    sem_wait(&roomLock);
    int opp = getRoom(id>=100?0:100);
    int same = getRoom(id>=100?100:0);
    int minID = opp==-1?same:opp;
    if (minID==-1){
        printf("%d, Done. Nobody to give the room to.\n",id);
    }else{
        printf("%d. Done. Giving up room %d to %d\n", id, room, minID);
        rooms[room]=minID;
    }
    sem_post(&roomLock);
    running--;
}
int main(int argc, char** argv){
    printf("Started\n");
    for (int i=0; i<10; i++){ rooms[i]=-1;}
    sem_init(&roomLock, 0,1);
    pthread_t pirate;
    for (int i=0; i<numN; i++){pthread_create(&pirate, NULL,run, (void *)i+100);}
    for (int i=0; i<numP; i++){ pthread_create(&pirate, NULL,run, (void *)i);}
    running +=numP;
    running +=numN;
    while (running!=0){sleep(1);}
    for (int i=0; i<200; i++){printf("%d , %d\n", i, cost[i]);}
    int ninjaCost=0, pirateCost=0;
    for (int i=0; i<100; i++){ pirateCost+=cost[i]; }
    for (int i=100; i<200; i++){ ninjaCost+=cost[i]; }
    printf("Pirate cost: %d\n",pirateCost);
    printf("Ninja cost: %d\n", ninjaCost);
    printf("Ended\n");
}
