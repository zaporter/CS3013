#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#define MAX_CMD_LENGTH 1000
static int *pid=0;
int BGID=-1;
static int *numBG=0;
char pwd[128];
char* statscmd = "/usr/bin/time -f ";
char* statsformat = "\n-- Statistics --\nElapsed time: \%e seconds\nPage Faults: \%F\nPage Faults (reclaimed): \%R\n-- End of Statistics --\n";

void init(){
    pwd[0] = '.';
    pwd[1]='\0';
    numBG = mmap(NULL, sizeof *numBG, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *numBG = 0;
    pid = mmap(NULL, sizeof *pid, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *pid = 0;
}

pid_t run(char* cmd, bool shouldwait){
    printf("Running command: %s\n",cmd);
    pid_t child;
    char fullCmd[MAX_CMD_LENGTH];
    int statusID;
    if (strlen(cmd)>4 && strncmp("ccd ", cmd, 4)==0){
        int i=4;
        while (i<strlen(cmd)){
            pwd[i++-4]=cmd[i];
        } 
        printf("Changed to directory: %s\n\n",pwd);
        return 0;
    }
    if (strlen(cmd)>=4 && strncmp("cpwd",cmd,4)==0){
        int i=0;
        printf("Current directory: %s\n\n",pwd);
        return 0;
    }
    if (strlen(cmd)>=9 && strncmp("cproclist",cmd,9)==0){
        printf("-- Background Processes --\n");
 //       printf("COMMAND: %s\n",command);
        if ((child=fork())==0) {
        char* command = "ps  | grep -v \" sh\\| multi\\| time\\| a.out\\| ps\\| grep\\| cut\\| xargs\\| bash\" | cut -c 24- | xargs sh -c 'number=0;for arg do printf \"[$number] $arg\\n\"; number=$((number+1)); done'";
            printf("%s\n",command);
            execl("/bin/sh","sh","-c",command, (char *) NULL);
        }else{
            while(shouldwait && waitpid(child, &statusID, 0) == -1){} // wait for completion
        }
        return 0;
        
    }
    if (!shouldwait) BGID++;
    switch(child = fork()){
        case 0: 
         //   printf("Running %s\n",fullCmd);
        // Prepends "cd $pwd; " and stats to the command
            if (shouldwait) sprintf(fullCmd,"cd %s; %s '%s' %s",pwd,statscmd,statsformat,cmd);
            else {
                while (*pid==0){};
                sprintf(fullCmd,"cd %s; %s '\n-- Job Complete [%d:%s] --\nProcess ID: %d\n%s' %s",pwd,statscmd,BGID,cmd,*pid,statsformat,cmd);
            }
            *numBG=*numBG+1;
            pid_t child2=0;
            int statusID2=0;
            if ((child2=fork())==0){
                execl("/bin/sh","sh","-c",fullCmd,(char *)NULL);
            }else{
                while(waitpid(child2, &statusID2, 0) == -1){} // wait for completion
                *numBG=*numBG-1;
                // kill the monitor
                kill(getpid(),SIGINT);
            }
        default:
            while(shouldwait && waitpid(child, &statusID, 0) == -1){} // wait for completion
            if (!shouldwait)  printf("Background: ID [%d]: %s\n\n",BGID,cmd);
            *pid=child;
    }
    return child;
}
void finish(){
   munmap(pid, sizeof *pid);
   while(*numBG>0){}
}