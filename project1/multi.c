#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
// define global variables (saves a few lines)
// bgProcesses will indicate which background processes are running
static int *bgProcesses;
// bgNames stores each of these process names
// *line stores the line for reading the file
char bgNames[32][128], *line;
int child, status, i, line_len;
void run(char* cmd, int shouldWait,int localProc){
    printf("\nRunning command: %s\n",cmd);
    // print if background
    if (!shouldWait) printf("Background: ID [%d]: %s\n",localProc,cmd);
    // create local variables -- same as custom.c
    char *args[1024], cmdTmp[1024];
    struct timeval beginning, end;
    struct rusage usagestats;
    strcpy(cmdTmp,cmd);
    // if ccd
    if (strlen(cmd)>4 && strncmp(cmd,"ccd ",4)==0){
        for (int k=0; k<4; k++){*cmd++;}
        chdir(cmd);
        printf("Changed to directory: %s   [Only output from ccd]\n",cmd);
    // if cpwd
    }else if (strlen(cmd)>=4 && strncmp(cmd,"cpwd",4)==0){
        printf("Current directory: %s    [Only output from cpwd]\n", getcwd(cmdTmp,1000));
    // if cproclist
    }else if (strlen(cmd)>=9 && strncmp(cmd,"cproclist",9)==0){
        // print each process if active
        for (int k=0; k<32; k++){
            if ((*bgProcesses) & (1<<k)) printf("[%d] %s\n",k,bgNames[k]);
        }
    }else{
        // same as custom.c
        for (i=0; cmd[i]; cmd[i]==' ' ? i++: *cmd++){}
        if (strlen(cmdTmp)>128 || i>32) printf("Max cmd length is 128 with 32 args!\n");
        for(i=0; (args[i]=strtok(i==0?cmdTmp:NULL," "))!=NULL; i++){}
        // set its own bit
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
                // unset the childs bit & print its information
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
    int pos, k,i=1;
    // make the variable global across processes
    bgProcesses = mmap(NULL, sizeof *bgProcesses, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *bgProcesses = pos = 0;
    while (getline(&line,(size_t*)(&line_len),fp)!=-1){
        line[strlen(line)-1]='\0';
        // check to see if it is contained within the list of processes to run in the background
        for (k=0; (k+1)<argc; k++){
            if (!(i==atoi(argv[k+1]))) continue;
            // copy the first 127 characters into bgNames
            strncpy(bgNames[pos % 32],line,127);
            run(line, 0, pos % 32);
            pos++;
            // indicate that this process ran in the background (does not support more than 999999 background processes)
            k=999998;
        }
        // run if not run as background process
        if (k!=999999) run(line, 1, 0);
        i++;
    }
    // wait for the children to finish
    while (*bgProcesses){}
}
