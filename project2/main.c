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
    int emptyPageLoc = getNextPage()*16;
    if (pidPageTables[pid]==-1){
        if (emptyPageLoc<0){
            printf("Not enough empty pages \n");   
            return 0;
        }
        claimPage(emptyPageLoc/16);
        pidPageTables[pid]=emptyPageLoc;
        printf("Claiming physical page %d for map\n",emptyPageLoc);
    }
    for (int x=0; x<4; x++){
        memory[emptyPageLoc+x]=0;
    }
    int VPN = addr >> 6; // pop off the top 2
    if (!memory[pidPageTables[pid] + VPN]){
        emptyPageLoc=getNextPage()*16;
        claimPage(emptyPageLoc/16);
        printf("Claiming page at physical address %d\n",emptyPageLoc);
        memory[pidPageTables[pid]+VPN] = emptyPageLoc/16;
        printf("memVal: %d\n",memory[pidPageTables[pid]]);
        //valid
        SET_BIT(memory[pidPageTables[pid]+VPN],7);
        // present in memory
        SET_BIT(memory[pidPageTables[pid]+VPN],5);
        if (value){// writeable
            SET_BIT(memory[pidPageTables[pid]+VPN],6);
        }
        printf("memVal: %d\n",memory[pidPageTables[pid]]);
    }
    return 1;
}
int store(int pid, int addr, int value){
    int VPN = addr >> 6; // pop off the top 2
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
    return 1;
}
int read(int pid, int addr, int value){
    int VPN = addr >> 6;
    int startOfPage=(memory[pidPageTables[pid]+VPN] & 0x03)*16;
    int offset = addr & 0x3F;
    return memory[startOfPage+offset];
}
int swap(int dontSwapPFN){
    
}
int main(){
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
