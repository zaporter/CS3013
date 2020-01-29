#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
int bgProcesses;
char bgNames[32][128];
int child, status, i;
void run(char* cmd, int shouldWait,int localProc){
    printf("Running command: %s\n",cmd);
    if (shouldWait){
        printf("WAITING FOR THIS PROCESS\n");
    }else{
        printf("NOT WAITING FOR THIS PROCESS\n");
    }
    char *args[1000], cmdTmp[1000];
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
    }else if (strlen(cmd)>=9 && strncmp(cmd,"cproclist",9)==0){
        
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
        while(shouldWait && waitpid(child,&status,0)==-1){}
    }
}
int main(int argc, char **argv){
    FILE * fp=fopen("multi.txt","r");
    char* line=NULL;
    size_t line_len=0;
    int pos=0;
    i=1;
    printf("started\n");
    while (getline(&line,&line_len,fp)!=-1){
        printf("fra\n");
        line[strlen(line)-1]='\0';
        if (pos+1<argc && i==atoi(argv[pos+1])){
            run(line, 0, pos);
            pos++;
        }else{
            run(line, 1, pos);
        }
        i++;
    }
}
