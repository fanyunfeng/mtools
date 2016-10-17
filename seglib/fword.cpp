#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <vector>

#include "tools.h"


#define CHECK_RETURN(ret)   \
        if(ret){            \
            return ret;     \
        }                   \


struct AppContext{
    FILE* fin;
    FILE* fout;

    const char* dictFile;
    const char* sep;

    int all;
    int index;

    const char* hint;
    std::vector<std::string> dict;
    std::vector<std::string>::iterator iterator;

    AppContext(){
        fin = NULL;
        fout = NULL;

        dictFile = NULL;

        all = 0;
        index = 0;
        hint = NULL;

        sep = "\t";
    }
};

int find(char* buf, AppContext& ctx){
    for(std::vector<std::string>::iterator it=ctx.iterator; it!=ctx.dict.end(); ++it){
        std::string& str = *it;
        ctx.hint = str.c_str();

        if(strstr(buf, ctx.hint)){
            ++ ctx.index;
            ctx.iterator = ++ it;
            return 0;
        }
    }

    return 1;
}

int loadWords(AppContext& ctx){
    int read = 0;
    char* end = NULL;

    char* line = NULL;
    size_t len = 0;

    FILE* file = fopen(ctx.dictFile, "r");

    if(file == NULL){
        return 1;
    }

    while ((read = getline(&line, &len, file)) != -1) {
        end = strEndl(line);

        if(end == line){
            continue;
        }

        ctx.dict.push_back(std::string(line));
    }

    ctx.iterator = ctx.dict.begin();

    free(line);
    fclose(file);
    return 0;
}

void reset(AppContext& ctx){
    ctx.hint = NULL;
    ctx.index = 0;
    ctx.iterator = ctx.dict.begin();
}

int process(AppContext& ctx){
    int result;

    int read = 0;
    char* end = NULL;
    char* line = NULL;
    size_t len = 0;

    while ((read = getline(&line, &len, ctx.fin)) != -1) {
        end = strEndl(line);

        if(end == line){
            continue;
        }

        while(true){
            result = find(line, ctx);

            if(result==0){
                if(ctx.index == 1){
                    fprintf(ctx.fout, "%s%s%s", line, ctx.sep, ctx.hint);
                }
                else{
                    fprintf(ctx.fout, " %s", ctx.hint);
                }

                if(ctx.all){
                    continue;
                }
                else{
                    break;
                }
            }
            else {
                break;
            }
        }

        if(ctx.index){
            fprintf(ctx.fout, "\n");
            reset(ctx);
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
}

void help(){
    fprintf(stdout, "find word.\n");

    fprintf(stdout, "    -i input text file.\n");
    fprintf(stdout, "    -o output text file.\n");
    fprintf(stdout, "    -w word file.\n");
    fprintf(stdout, "    -g find all.\n");
    fprintf(stdout, "    -t seperator. [\\t].\n");
}


int parseArgs(int argc, char* argv[], AppContext& ctx){
    int opt;
    char* ifile=NULL;
    char* ofile=NULL;

    while ((opt = getopt(argc, argv, "gw:i:o:t:")) != -1) {
        switch (opt) {
        case 'g':
            ctx.all = 1;
            break;
        case 'w':
            ctx.dictFile = optarg; 
            break;
        case 'o':
            ofile= optarg;
            break;
        case 'i':
            ifile= optarg;
            break;
        case 't':
            ctx.sep = optarg;
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

    ret = loadWords(ctx);
    CHECK_RETURN(ret);

    ret = process(ctx);

    return ret;
}
