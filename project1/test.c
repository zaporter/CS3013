#include "test_helper.c"

void main(){
    printf("Started\n");
    init();
    run ("sleep 10",false);
    run ("ls -alh",true);
    run ("sleep 3",false);
    run ("cproclist",true);
 //   run ("ps",true);
    run ("cpwd",true);
 /*   run("sleep 5",false);
    run("ccd /home/zack/Tools",true);
    run("ls -alh",true);
    run("cpwd ",true);
    run ("ps",true);
    run("cproclist",true); */
 //  run("ps | grep -v \"sh\\|multi\\|time\\|grep\\|ps\\|PID\" | cut -d' ' -f8",true);
    finish();
    printf("Ended\n");
}

