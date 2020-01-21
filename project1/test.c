#include "zrunner.c"

void main(){
    printf("Started\n");
    init();
    run("sleep 5",false);
    run("ccd /home/zack/Tools",true);
    run("ls -alh",true);
    run("cpwd ",true);
    run("firefox",false);
    run("ps",true);
    printf("Ended\n");
}
