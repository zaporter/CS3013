#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
void run(char* cmd){
    char *args[1000], cmdTmp[1000];
    strcpy(cmdTmp,cmd);
    for(int i=0; (args[i]=strtok(i==0?cmdTmp:NULL," "))!=NULL; i++){}
    int child, status;
    struct timeval beginning, end;
    struct rusage usagestats;
    if ((child=fork())==0){
        printf("Running command: %s\n",cmd);
        gettimeofday(&beginning,NULL);
        if ((child=fork())==0){
            execvp(args[0],args);
            printf("Exec failed -- Command: %s\n",cmd);
        }else{
            while(waitpid(child,&status,0)==-1){}
            gettimeofday(&end,NULL);
            getrusage(child,&usagestats);
            printf("\n-- Statistics ---\nElapsed time: %d milliseconds\nPage Faults: %ld\nPage Faults (reclaimed): %ld\n-- End of Statistics --\n\n",(int)(1000*(end.tv_sec - beginning.tv_sec)+(end.tv_usec-beginning.tv_usec)/1000),usagestats.ru_majflt, usagestats.ru_minflt);
            kill(getpid(),SIGINT);           
        }
    }else{
        while(waitpid(child,&status,0)==-1){}
    }
}
int main(){
    run("whoami");
    run("last");
    run("ls -al");
}
