#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
void run(char* cmd){
    printf("Running command: %s\n",cmd);
    char *args[1000], cmdTmp[1000];
    int child, status, i;
    struct timeval beginning, end;
    struct rusage usagestats;
    strcpy(cmdTmp,cmd);
    if (strlen(cmd)>4 && strncmp(cmd,"ccd ",4)==0){
        for (int k=0; k<4; k++){*cmd++;}
        chdir(cmd);
        printf("Changed to directory: %s   [Only output from ccd]\n\n",cmd);
        return;
    }else if (strlen(cmd)>=4 && strncmp(cmd,"cpwd",4)==0){
        printf("Current directory: %s    [Only output from cpwd]\n\n", getcwd(cmdTmp,1000));
        return;
    }
    for (i=0; cmd[i]; cmd[i]==' ' ? i++: *cmd++){}
    if (strlen(cmdTmp)>128 || i>32) printf("Max cmd length is 128 with 32 args!\n");
    for(i=0; (args[i]=strtok(i==0?cmdTmp:NULL," "))!=NULL; i++){}
    if ((child=fork())==0){
        gettimeofday(&beginning,NULL);
        if ((child=fork())==0){
            execvp(args[0],args);
            printf("Exec failed\n");
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
    FILE *fp = fopen("custom.txt","r");
    char* line=NULL;
    size_t len=0;
    while (getline(&line,&len,fp)!=-1){
        line[strlen(line)-1]='\0';
        run(line);
    }
}
