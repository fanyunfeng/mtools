#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "tools.h"

#include <DRWordSeg/HCSegDll.h>

#define CHECK_RETURN(ret)   \
        if(ret){            \
            return ret;     \
        }                   \


struct SegmentContext{
    char* path;
    void* dictionary;
    void* handle;

    size_t len;
    char* buf;
};

struct AppContext{
    FILE* fin;
    FILE* fout;

    char* dictionary;
    int ignoreBlankLine;

    SegmentContext segmentContxt;

    AppContext(){
        memset(this, 0, sizeof(AppContext));
    }
};

char* split(char* buf, SegmentContext& ctx){
    size_t len = strlen(buf);

    len *= 10;
    len ++;

    if(ctx.len < len){
        char* ret = (char*) realloc(ctx.buf, len);
        if(ret == 0){
            exit(1);
        }
        else{
            ctx.len = len;
            ctx.buf = ret;
        }
    }

    HCSegBuf(buf, strlen(buf), ctx.buf, ctx.len, ctx.handle, 0, 1, 0);

    return ctx.buf;
}

int loadSegLib(const char* path, SegmentContext& lib){
    int ret = 0;

    size_t len = 1024;
    char* buf = (char*) malloc(len);

    while(getcwd(buf, len)==0 && errno == ERANGE){
        len *= 2;
        char* ret = (char*) realloc(buf, len);

        if(ret == 0){
            free(buf);
            exit(1);
        }
    }

    if(ret=chdir(path)){
        return ret;
    }

    if(GetDictHandle(lib.dictionary, 1)){
        return 1;
    }

    if(ret=chdir(buf)){
        return ret;
    }

    free(buf);

    if(GetSegHandle(lib.dictionary, lib.handle)){
        return 1;
    }

    return 0;
}

void destorySegLib(SegmentContext& lib){
    FreeSegHandle(lib.handle);
    FreeDictHandle(lib.dictionary);
}

int splitFile(AppContext& ctx){
    char* result;

    int read = 0;
    char* end = NULL;
    char* line = NULL;
    size_t len = 0;

    while ((read = getline(&line, &len, ctx.fin)) != -1) {
        end = strEndl(line);

        if(end == line){
            if(ctx.ignoreBlankLine){
                continue;
            }
            else{
                fprintf(ctx.fout, "\n");
                continue;
            }
        }


        result = split(line, ctx.segmentContxt);

        if(result){
            fprintf(ctx.fout, "%s\n", result);
        }
        else{
            fprintf(ctx.fout, "\n");
        }
    }

    free(line);

    return 0;
}

void doExit(AppContext& ctx){
    if(ctx.fin){
        fclose(ctx.fin);
    }

    if(ctx.fout){
        fclose(ctx.fout);
    }

    
    //free(ctx.dictionary);
}

void help(){
    fprintf(stdout, "split text by dictionary.\n");

    fprintf(stdout, "    -i input text file.\n");
    fprintf(stdout, "    -b output text file.\n");
    fprintf(stdout, "    -d dictionary directory.\n");
    fprintf(stdout, "    -b ignore blank line.\n");
}


int parseArgs(int argc, char* argv[], AppContext& ctx){
    int opt;
    char* ifile=NULL;
    char* ofile=NULL;

    while ((opt = getopt(argc, argv, "bd:i:o:")) != -1) {
        switch (opt) {
        case 'b':
            ctx.ignoreBlankLine = 1;
            break;
        case 'd':
            ctx.dictionary = optarg; 
            break;
        case 'o':
            ofile= optarg;
            break;
        case 'i':
            ifile= optarg;
            break;
        default:
            help();
            exit(0);
            break;
        }
    }

    if(ifile==NULL || strcmp(ifile, "-")==0){
        ctx.fin = stdin; 
    }
    else{
        ctx.fin = fopen(ifile, "r");

        if(ctx.fin==NULL){
            return 1;
        }
    }

    if(ofile==NULL){
        ctx.fout = stdout;
    }
    else{
        ctx.fout = fopen(ofile, "w");
        if(ctx.fout==NULL){
            return 1;
        }
    }

    return 0;
}

int main(int argc, char* argv[]){
    int ret;
    AppContext ctx;

    ret = parseArgs(argc, argv, ctx);

    CHECK_RETURN(ret);

    loadSegLib(ctx.dictionary, ctx.segmentContxt);

    ret = splitFile(ctx);

    destorySegLib(ctx.segmentContxt);
    doExit(ctx);
    return ret;
}
