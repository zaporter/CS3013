#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
void run(char* cmd){
    printf("Running command: %s\n",cmd);
    // define space for arguments and temporary storage for the command
    char *args[1024], cmdTmp[1024];
    // copy the command into the temp storage
    strcpy(cmdTmp,cmd);
    // populate args array
    for(int i=0; (args[i]=strtok(i==0?cmdTmp:NULL," "))!=NULL; i++){}
    // define local variables
    int child, status;
    struct timeval beginning, end;
    struct rusage usagestats;
    // create child process
    if ((child=fork())==0){
        // save current time
        gettimeofday(&beginning,NULL);
        // create grandchild
        if ((child=fork())==0){
            execvp(args[0],args);
            printf("Exec failed -- Command: %s\n",cmd);
        }else{
            // wait for the process to finish
            while(waitpid(child,&status,0)==-1){}
            // save time of day
            gettimeofday(&end,NULL);
            // get usage of the grandchild (Does not include those of other grandchildren)
            getrusage(RUSAGE_CHILDREN,&usagestats);
            // print statistics
            printf("\n-- Statistics ---\nElapsed time: %d milliseconds\nPage Faults: %ld\nPage Faults (reclaimed): %ld\n-- End of Statistics --\n\n",(int)(1000*(end.tv_sec - beginning.tv_sec)+(end.tv_usec-beginning.tv_usec)/1000),usagestats.ru_majflt, usagestats.ru_minflt);
            // kill child
            kill(getpid(),SIGINT);           
        }
    }else{
        // wait for child to kill itself
        while(waitpid(child,&status,0)==-1){}
    }
}
int main(){
    run("whoami");
    run("last");
    run("ls -al");
}
