#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
static char** *glob_var;

int main(void)
{
    glob_var = mmap(NULL, 6*(sizeof(char*)), PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    

    if (fork() == 0) {
        
        char* hi=(char*)malloc(100*sizeof(char));
        strcpy(hi,"yelp");
        *glob_var[0] = hi;
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
        printf("%s\n", *glob_var[0]);
        munmap(glob_var, sizeof *glob_var);
    }
    return 0;
}
