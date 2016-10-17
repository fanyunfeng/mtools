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
#include "aho_corasick/aho_corasick.hpp"

using namespace aho_corasick;


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

struct AppContext{
    FILE* fin;
    FILE* fout;

    const char* dictFile;
    const char* sep;

    int all;
    int index;

    wchar_t wline;
    size_t wlen;

    const char* hint;
    std::vector<std::string> dict;
    std::vector<std::string>::iterator iterator;

    aho_corasick::wtrie trie;

    MemBlock memBlock;

    AppContext(){
        fin = NULL;
        fout = NULL;

        dictFile = NULL;

        all = 0;
        index = 0;
        hint = NULL;

        sep = "\t";

        trie.remove_overlaps();
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
    auto emits = ctx.trie.parse_text(str);

    auto it = emits.begin();

    if(emits.empty()){
        if(ctx.all){
            fprintf(ctx.fout, "%s\n",  buf);
        }
        
        return 1;
    }

    fprintf(ctx.fout, "%s%s",  buf, ctx.sep);

    int first = 1;
    while(it != emits.end()){
        if(first){
            printWString(it->get_keyword(), ctx);
        }
        else{
            fprintf(ctx.fout, " ");
            //fwprintf(ctx.fout, L"%s", it->get_keyword().c_str());
            printWString(it->get_keyword(), ctx);
        }

        if(!ctx.all){
            break;
        }

        first = 0; 
        ++ it;
    }

    fprintf(ctx.fout, "\n");
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

        std::wstring wbuf = toWString(line, (size_t)(end-line));

        ctx.trie.insert(wbuf);
        //ctx.dict.push_back(std::string(line));
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
    int read = 0;
    char* end = NULL;
    char* line = NULL;
    size_t len = 0;

    while ((read = getline(&line, &len, ctx.fin)) != -1) {
        end = strEndl(line);

        if(end == line){
            continue;
        }

        std::wstring buf = toWString(line, end-line);

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
