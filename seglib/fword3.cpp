#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <vector>

#include <malloc.h>
#include "tools.h"
#include <hcac/MultiModeMatch.h>


#define CHECK_RETURN(ret)   \
        if(ret){            \
            return ret;     \
        }                   \


std::wstring toWString(const char* str, size_t len) {
    std::wstring ret;
    wchar_t ch;
    size_t i = 0;

    for(const unsigned char* p=(const unsigned char*)str; *p; ++p, ++i){
        if(*p < 0x80){
            ch = *p;
        }
        else {
            ch = *(p ++);
            ch += (*(p) << 8);
        }

        ret.push_back(ch);
    }

    return ret;
}

struct MemBlock{
    char* buffer;
    size_t size;

    MemBlock():buffer(NULL), size(0){
    }

    int realloc(size_t _size){
        char* buf = (char*) ::realloc(buffer, _size);

        if(buf == NULL){
            return 1;
        }

        size = _size;
        buffer = buf;

        return 0;
    }
};

#define KEYWORDS_SCALE 601

struct AppContext{
    FILE* fin;
    FILE* fout;

    const char* dictFile;
    const char* sep;

    int all;
    int index;

    wchar_t wline;
    size_t wlen;

    void* lib;

    const char* hint;
    std::vector<std::string> dict;
    std::vector<std::string>::iterator iterator;

    MemBlock memBlock;

    char *result;
    char **patrns;
    int *freqs;

    AppContext(){
        fin = NULL;
        fout = NULL;

        dictFile = NULL;

        all = 0;
        index = 0;
        hint = NULL;

        sep = "\t";

        result = new char[20*1024];
        patrns = new char*[KEYWORDS_SCALE];
        freqs = new int[KEYWORDS_SCALE];

        for(int i=0;i<KEYWORDS_SCALE;i++) {
            patrns[i] = new char[100];
            memset(patrns[i],0,sizeof(100));
            freqs[i]=0;
        }
        result[0]='\0';
    }
};

void printWString(const std::wstring& str, AppContext& ctx){
    if(ctx.memBlock.realloc(str.length() * 2 + 1)){
        return;
    }
    
    char* p = ctx.memBlock.buffer;

    for(std::wstring::const_iterator it=str.begin(); it!=str.end(); ++it){
        if(*it<0){
            *p++ = *it;
        }
        else{
            *p++ = (*it) & 0xFF;
            *p++ = ((*it) >> 8) & 0xFF;
        }
    }

    *p = 0;

    fprintf(ctx.fout, "%s", ctx.memBlock.buffer);
}

int find(const char* buf, const std::wstring& str, AppContext& ctx){
    char* pResult = 0;

    ac_Search(const_cast<char*>(buf), pResult, ctx.result, ctx.patrns, ctx.freqs, ctx.lib);

    if(pResult){
        ac_FreeMem(pResult);
        fprintf(ctx.fout, "%s%s",  buf, ctx.sep);

        fprintf(ctx.fout, "\n");
    }
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

    std::string istr;

    while ((read = getline(&line, &len, file)) != -1) {
        end = strEndl(line);

        if(end == line){
            continue;
        }

        istr.append(line);
        istr.append("\x08");
    }

    ac_GetPatternTree(const_cast<char*>(istr.c_str()), ctx.lib);

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
    int read = 0;
    char* end = NULL;
    char* line = NULL;
    size_t len = 0;

    std::wstring buf; // = toWString(line, end-line);
    while ((read = getline(&line, &len, ctx.fin)) != -1) {
        end = strEndl(line);

        if(end == line){
            continue;
        }

        //std::wstring buf = toWString(line, end-line);

        find(line, buf, ctx);
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
