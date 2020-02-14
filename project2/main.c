#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PROCESS_ID 0
#define INSTRUCTION_TYPE 1
#define VIRTUAL_ADDRESS 2
#define VALUE 3
#define CLEAR_BIT(S, B) S &= ~(1 << B)
#define SET_BIT(S, B) S |= (1 << B)

u_int8_t memory[64];
int8_t pidPageTables[4];
int32_t claimedPages[4];

char input[4][32];

FILE *file;
int getNextPage(){
    for (int i=0; i<4; i++){
        if (!claimedPages[i]){
            return i;
        }
    }
    return -1;
}
void claimPage(int loc){
    claimedPages[loc]=1;
}
int freePages(){
    int i=0;
    for (int x=0; x<4; x++){
        if (!claimedPages[x]){
            i++;
        }
    }
    return i;
}

void sflush(){
    printf("Flushing\n");
    for (int i=0; i<4; i++){
        if (pidPageTables[i]!=-1)
            swapout(i);
    }
    for (int x=0; x<64; x++){
        memory[x]=0;
    }
}
void swapin(int pid, int VPN){
    printf("Swapping in %d & %d\n",pid*5,pid*5+VPN+1);
    
    if (freePages()<2){
        sflush();
    }   
    file=fopen("swap","r");
    u_int8_t tmpTBL[16];
    u_int8_t tmpPG[16];
    for (int i=0; i<pid*5+1; i++){
        fread(&tmpTBL, 16, 1,file);
    }
    for (int i=0; i<VPN+1; i++){
        fread(&tmpPG, 16, 1, file);
    }
    int emptyPageLoc = getNextPage()*16;
    claimPage(emptyPageLoc/16);
    int emptyPage2Loc = getNextPage()*16;
    claimPage(emptyPage2Loc/16);
        pidPageTables[pid]=emptyPageLoc;
        printf("Claiming physical page %d for map\n",emptyPageLoc);
    if (pidPageTables[pid]==-1){
    }
    fclose(file);
    for (int x=0; x<16; x++){
        memory[emptyPageLoc+x] = tmpTBL[x];
    }
    for (int x=0; x<16; x++){
        memory[emptyPage2Loc+x] = tmpPG[x];
    }
    
    for (int x=0; x<4; x++){
        memory[emptyPageLoc+x] &= 0xC0; // clear  the vpn and present
    }
    // present in memory
    SET_BIT(memory[pidPageTables[pid]+VPN],5);
    memory[emptyPageLoc+VPN] |= emptyPage2Loc/16;
    printf("MAP VAL:%d other %d\n",memory[emptyPageLoc+VPN],emptyPage2Loc/16);
    printf("Swapped in\n");
}
void swapclean(int frame, int swapFrame){
    printf("Clean swapping %d to %d\n",frame,swapFrame);
    file=fopen("swap","r+");
    fseek(file, 16*(swapFrame),SEEK_SET);
    for (int x=frame*16; x<(frame+1)*16; x++){
        fwrite(&memory[x],1,1,file);
//        printf("wrote: %d\n",memory[x]);
        memory[x]=0;
    }   
    fclose(file);
    claimedPages[frame]=0;
}
int swapout(int pid){
    printf("Swapping out pid: %d\n", pid);
    for (int VPN=0; VPN<4; VPN++){
            printf("reading: %d\n",pidPageTables[pid]+VPN);
            printf("val: %d\n",memory[pidPageTables[pid]+VPN]);
        // is present
        if ((memory[pidPageTables[pid]+VPN] & 0x20)){
            int page=(memory[pidPageTables[pid]+VPN] & 0x03);
            swapclean(page, pid*5+VPN+1);
        }
    }
    swapclean(pidPageTables[pid], pid*5);
    pidPageTables[pid]=-1;
}

void getInput(){
    int inNum=0;
    int loc=0;
    char current[1024];
    char tmp;
    do{
        tmp=getchar();
        if (tmp==',' || tmp == '\n'){
            strncpy(input[inNum++],current,loc);
            input[inNum-1][loc]='\0';
            loc=0;
        }else{
            current[loc++]=tmp;
        }
    }while(tmp!='\n');
}
int map(int pid, int addr, int value){
    swapin(pid,addr>>4);
    int VPN = addr >> 4; // pop off the top 2
    printf("VPN IS :%d\n",VPN);
   // if (!memory[pidPageTables[pid] + VPN]){
        printf("memVal: %d\n",memory[pidPageTables[pid]]);
        //valid
        SET_BIT(memory[pidPageTables[pid]+VPN],7);
        if (value){// writeable
            SET_BIT(memory[pidPageTables[pid]+VPN],6);
        }
        printf("memVal: %d\n",memory[pidPageTables[pid]]);
   // }
    sflush();
    return 1;
}
int store(int pid, int addr, int value){
    swapin(pid,addr>>4);
    int VPN = addr >> 4; // pop off the top 2
    // test valid
    printf("Checking memory loc %d\n",pidPageTables[pid]+VPN);
    if (!(memory[pidPageTables[pid]+VPN] & 0x80)){
        printf("Invalid address in page! Have you mapped this page yet?\n");
        return 0;
    }
    // test writeable
    if (!(memory[pidPageTables[pid]+VPN] & 0x60)){
        printf("Page not writable\n");
        return 0;
    }
    int startOfPage=(memory[pidPageTables[pid]+VPN] & 0x03)*16;
    int offset = addr & 0x3F;
    memory[startOfPage+offset]=value;
    sflush();
    return 1;
}
int read(int pid, int addr, int value){
    swapin(pid, addr>>4);
    int VPN = addr >> 4;
    // is valid
    if (!(memory[pidPageTables[pid]+VPN] & 0x80)){
        printf("Invalid address in page! Have you mapped this page yet?\n");
        return -1;
    }
    int startOfPage=(memory[pidPageTables[pid]+VPN] & 0x03)*16;
    int offset = addr & 0x3F;
    int val =  memory[startOfPage+offset];
    sflush();
    return val;
}
int main(){
    printf("Creating swap file");
    file=fopen("swap","w");
    int zero=0;
    for (int i=0; i<20*16; i++){
        fwrite(&zero,1,1,file);
    }
    fclose(file);
    for (int x=0; x<4; x++){
        pidPageTables[x]=-1;
    }
    char * pEnd;
    printf("Starting\n");
    while (1){
        printf("Input: ");
        getInput();
        if (strlen(input[1])>=3 && strncmp(input[1],"map",3)==0){
            if (map(atoi(input[0]), atoi(input[2]), atoi(input[3])))
                printf("Mapped\n");
        }
        else if (strlen(input[1])>=5 && strncmp(input[1],"store",5)==0){
            if (store(atoi(input[0]), atoi(input[2]), atoi(input[3])))
                printf("Stored\n");
        }
        else if (strlen(input[1])>=4 && strncmp(input[1],"read",4)==0){
            printf("Read: %d\n",read(atoi(input[0]), atoi(input[2]), atoi(input[3])));

        }else{
            printf("Invalid command\n");
        }
    }
    printf("Ending\n");
}
