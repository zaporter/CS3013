#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
void run(char* cmd){
    printf("Running command: %s\n",cmd);
    // create local variables
    char *args[1024], cmdTmp[1024];
    int child, status, i;
    struct timeval beginning, end;
    struct rusage usagestats;
    strcpy(cmdTmp,cmd);
    // if the command is ccd  
    if (strlen(cmd)>4 && strncmp(cmd,"ccd ",4)==0){
        // pop the pointer up by 4 to remove "ccd "
        for (int k=0; k<4; k++){*cmd++;}
        chdir(cmd);
        printf("Changed to directory: %s   [Only output from ccd]\n\n",cmd);
        return;
    // if the command is cpwd
    }else if (strlen(cmd)>=4 && strncmp(cmd,"cpwd",4)==0){
        printf("Current directory: %s    [Only output from cpwd]\n\n", getcwd(cmdTmp,1000));
        return;
    }
    // else it is a normal command
    // count number of spaces (assuming each is an argument)
    for (i=0; cmd[i]; cmd[i]==' ' ? i++: *cmd++){}
    // check if they exceeded max length and print a warning if yes
    if (strlen(cmdTmp)>128 || i>32) printf("Max cmd length is 128 with 32 args!\n");
    // populate args array
    for(i=0; (args[i]=strtok(i==0?cmdTmp:NULL," "))!=NULL; i++){}
    // create child (same stuff as boring.c)
    if ((child=fork())==0){
        gettimeofday(&beginning,NULL);
        if ((child=fork())==0){
            execvp(args[0],args);
            printf("Exec failed -- Command: %s\n",cmd);
        }else{
            while(waitpid(child,&status,0)==-1){}
            gettimeofday(&end,NULL);
            getrusage(RUSAGE_CHILDREN,&usagestats);
            printf("\n-- Statistics ---\nElapsed time: %d milliseconds\nPage Faults: %ld\nPage Faults (reclaimed): %ld\n-- End of Statistics --\n\n",(int)(1000*(end.tv_sec - beginning.tv_sec)+(end.tv_usec-beginning.tv_usec)/1000),usagestats.ru_majflt, usagestats.ru_minflt);
            kill(getpid(),SIGINT);           
        }
    }else{
        // wait for child to kill itself
        while(waitpid(child,&status,0)==-1){}
    }
}
int main(){
    FILE *fp = fopen("custom.txt","r");
    char* line=NULL;
    size_t len=0;
    while (getline(&line,&len,fp)!=-1){
        // replace newline with null terminator
        line[strlen(line)-1]='\0';
        // run it
        run(line);
    }
}
