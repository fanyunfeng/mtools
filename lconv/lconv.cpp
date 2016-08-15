#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

const char* fromcode = NULL;
const char* tocode = NULL;

const char* inputFileName = NULL;
const char* outputFileName = NULL;

FILE* input=NULL;
FILE* output=NULL;

int bufSize = 1024*1024;

char* line=NULL;
size_t lineLen=0;

char* buf=NULL;
size_t bufLen=0;


void printHelp(const char* cmd){
    printf("%s [-i input] [-o output] -f <CoderName> -t <CoderName>\n", cmd);
    exit(1);
}

int prepInputFile(const char* name){
    if(name == NULL){
        input = stdin;
        return 0;
    }

    input = fopen(name, "r");

    if(input){
        return 0;
    }

    return 1;
}

int prepOutputFile(const char* name){
    if(name == NULL){
        output = stdout;
        return 0;
    }

    input = fopen(name, "w");

    if(input){
        return 0;
    }

    return 1;
}

void cleanupInputFile(){
    if(input != stdin){
        fclose(input);
    }
}

void cleanupOutputFile(){
    if(output != stdout){
        fclose(output);
    }
}

int prepMem(int size){
    if(size <=0){
        return 1;
    }

    line = (char*) malloc(size);
    lineLen = size;

    bufLen = size * 6;
    buf = (char*) malloc(bufLen);

    return 0;
}

void cleanupMem(){
    if(line){
        free(line);
    }

    if(buf){
        free(buf);
    }
}

void parseArgs(int argc, const char* argv[]){
    int i=1;
    for(;i<argc; i++){
        if(!strcmp(argv[i], "-f")){
            if(i+1<argc){
                ++ i;
                fromcode = argv[i];
                continue;
            }
        }
        else if(!strcmp(argv[i], "-t")){
            if(i+1<argc){
                ++ i;
                tocode = argv[i];
                continue;
            }
        }
        else if(!strcmp(argv[i], "-i")){
            if(i+1<argc){
                ++ i;
                inputFileName = argv[i];
                continue;
            }
        }

        break;
    }
}


int main(int argc, const char* argv[]){
    iconv_t conv = NULL;
    char* line=NULL;
    size_t lineLen;
    int read = 0;

    char* buf;
    int bufLen;

    char* in;
    size_t inLen;

    char* out;
    size_t outLen;

    parseArgs(argc, argv);

    if(fromcode == NULL || tocode == NULL){
        printHelp(argv[0]);
        return 1;
    }

    prepInputFile(inputFileName);
    prepOutputFile(outputFileName);
    prepMem(bufSize);

    conv = iconv_open(fromcode, tocode);

    if(conv < 0){
        return errno;
    }

    while ((read = getline(&line, &lineLen, input)) != -1) {
        for(int i=0; i<2; i++){
            in = line;
            inLen = lineLen;

            out = buf;
            outLen = bufLen;
            if(iconv(conv, &in, &inLen, &out, &outLen)<0){
                int code = errno;

                if(code == E2BIG){

                    continue;
                }
                else{
                    fprintf(stderr, "error:%s", line);
                }
            }

            fprintf(output, "%s", out);
            break;
        }
    }

    iconv_close(conv);

    cleanupMem();
    cleanupInputFile();
    cleanupOutputFile();

    return 0;
}
