#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
static int *bgProcesses;
char bgNames[32][128];
int child, status, i;
void run(char* cmd, int shouldWait,int localProc){
    printf("\nRunning command: %s\n",cmd);
    if (!shouldWait) printf("Background: ID [%d]: %s\n",localProc,cmd);
    char *args[1000], cmdTmp[1000];
    struct timeval beginning, end;
    struct rusage usagestats;
    strcpy(cmdTmp,cmd);
    if (strlen(cmd)>4 && strncmp(cmd,"ccd ",4)==0){
        for (int k=0; k<4; k++){*cmd++;}
        chdir(cmd);
        printf("Changed to directory: %s   [Only output from ccd]\n",cmd);
    }else if (strlen(cmd)>=4 && strncmp(cmd,"cpwd",4)==0){
        printf("Current directory: %s    [Only output from cpwd]\n", getcwd(cmdTmp,1000));
    }else if (strlen(cmd)>=9 && strncmp(cmd,"cproclist",9)==0){
        for (int k=0; k<32; k++){
            if ((*bgProcesses) & (1<<k)) printf("[%d] %s\n",k,bgNames[k]);
        }
    }else{
        for (i=0; cmd[i]; cmd[i]==' ' ? i++: *cmd++){}
        if (strlen(cmdTmp)>128 || i>32) printf("Max cmd length is 128 with 32 args!\n");
        for(i=0; (args[i]=strtok(i==0?cmdTmp:NULL," "))!=NULL; i++){}
        if (!shouldWait) (*bgProcesses) |= (1 << localProc);
        if ((child=fork())==0){
            gettimeofday(&beginning,NULL);
            if ((child=fork())==0){
                execvp(args[0],args);
                printf("Exec failed -- Command: %s\n",cmd);
            }else{
                while(waitpid(child,&status,0)==-1){}
                gettimeofday(&end,NULL);
                getrusage(RUSAGE_CHILDREN,&usagestats);
                if (!shouldWait) (*bgProcesses) &= ~(1 << localProc);
                if (!shouldWait) printf("\n-- Job Complete [%d: %s] --\nProcess ID: %d\n",localProc,bgNames[localProc],child);
                printf("\n-- Statistics ---\nElapsed time: %d milliseconds\nPage Faults: %ld\nPage Faults (reclaimed): %ld\n-- End of Statistics --\n",(int)(1000*(end.tv_sec - beginning.tv_sec)+(end.tv_usec-beginning.tv_usec)/1000),usagestats.ru_majflt, usagestats.ru_minflt);
                kill(getpid(),SIGINT);           
            }
        }else{
            while(shouldWait && waitpid(child,&status,0)==-1){}
    }}}
int main(int argc, char **argv){
    FILE * fp=fopen("multi.txt","r");
    char* line=NULL;
    size_t line_len=0;
    int pos, i=1;
    bgProcesses = mmap(NULL, sizeof *bgProcesses, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *bgProcesses = pos = 0;
    while (getline(&line,&line_len,fp)!=-1){
        line[strlen(line)-1]='\0';
        if (pos+1<argc && i==atoi(argv[pos+1])){
            strncpy(bgNames[pos % 32],line,127);
            run(line, 0, pos % 32);
            pos++;
        }else
            run(line, 1, 0);
        i++;
    }
    while (*bgProcesses){}
}
