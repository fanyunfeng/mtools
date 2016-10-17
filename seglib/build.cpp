#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

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
    const char* source;
    const char* destination;

    AppContext(){
        memset(this, 0, sizeof(AppContext));
    }
};

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

    chdir(path);

    if(GetDictHandle(lib.dictionary, 1)){
        return 1;
    }

    chdir(buf);
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

int build(AppContext& ctx){
    return CreateDictrionary(ctx.source, ctx.destination);
}

void doExit(AppContext& ctx){
}

void help(){
    fprintf(stdout, "build term dictionary.\n");

    fprintf(stdout, "    -s source path.\n");
    fprintf(stdout, "    -d destination path.\n");
}


int parseArgs(int argc, char* argv[], AppContext& ctx){
    int opt;

    while ((opt = getopt(argc, argv, "s:d:")) != -1) {
        switch (opt) {
        case 's':
            ctx.source = optarg;
            break;
        case 'd':
            ctx.destination = optarg;
            break;
        default:
            help();
            exit(0);
            break;
        }
    }

    if(ctx.source==NULL){
        ctx.source = ".";
    }

    if(ctx.destination==NULL){
        ctx.destination = ".";
    }

    return 0;
}

int main(int argc, char* argv[]){
    int ret;
    AppContext ctx;

    ret = parseArgs(argc, argv, ctx);

    CHECK_RETURN(ret);

    ret = build(ctx);

    doExit(ctx);
    return ret;
}
