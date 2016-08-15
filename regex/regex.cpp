#include "lib/CRegex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* reg=NULL;
const char* text=NULL;

int verbose = 0;

void printHelp(const char* cmd){
    printf("%s reg text\n", cmd);
    printf("\t-v verboase\n", cmd);
}

void parseArgs(int argc, const char* argv[]){
    int i = 1;
    for(; i<argc; i++){
        if(!strcmp(argv[i], "-v")){
            verbose = 1;
        }
        else{
            break;
        }
    }

    if(i<argc && reg==NULL){
        reg = argv[i];
        ++ i;
    }

    if(i<argc && text==NULL){
        text = argv[i];
        ++ i;
    }

    if(reg==NULL || text==NULL){
        printHelp(argv[0]);
        exit(1);
    }
}

int main(int argc, const char* argv[]){
    
    parseArgs(argc, argv);

    if(verbose){
        puts("regs:");
        puts(reg);

        puts("text:");
        puts(text);
    }
   
    CRegex regex(reg);


    int ret = regex.match(text, 0, 0);

    if(verbose){
        if(ret){
            puts("matched.");
        }
    }

    return ret;
}
