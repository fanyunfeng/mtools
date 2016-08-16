#ifndef _IO_APP_HH_
#define _IO_APP_HH_ 0

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

struct Buffer{
    char*   buf;
    size_t  len;
    size_t  size;

    Buffer(){
        buf = NULL;
        len = 0;
        size = 0;
    }

    ~Buffer(){
        if(buf){
            free(buf);
            buf = NULL;
        }
    }

    int intialize(size_t size){
        this->size = size;
        buf = (char*) malloc(size);

        return !buf;
    }
};

enum {
    PARSE_OK,
    PARSE_UNKNOWN,
    PARSE_ERROR
};

class IOApp{
public:
    int run(int argc, const char* argv[]){
        int ret = 0;
        if(_parseArgs(argc, argv)){
            return 1;
        }

        prepare();

        while(true){
            buf.len = getline(&buf.buf, &buf.size, in);

            if(buf.len == (size_t)-1){
                break;
            }

            if((ret = handleLine(buf))){
                return ret;
            }
        }
        return 0;
    }

    IOApp(){
        in = NULL;
        out = NULL;
    }

    virtual ~IOApp(){
        if(in!=stdin && in!=NULL){
            fclose(in);
            in = NULL;
        }

        if(out!=stdout && in!=NULL){
            fclose(out);
            out = NULL;
        }
    }
protected:
    virtual int prepare(){
        return 0;
    }

    virtual int handleLine(Buffer& ){
        return 0;
    }

    virtual int parseArgs(int argc, const char* argv[], int& i){
        if(!strcmp(argv[i], "-i")){
            if(i+1<argc){
                ++ i;
                in = fopen(argv[i], "r");
                if(in){
                    return PARSE_OK;
                }
            }
        }
        else if(!strcmp(argv[i], "-o")){
            if(i+1<argc){
                ++ i;
                out = fopen(argv[i], "w");
                if(out){
                    return PARSE_OK;
                }
            }
        }
        else if(!strcmp(argv[i], "-m")){
            if(i+1<argc){
                ++ i;

                int s = atoi(argv[i]);

                if(s<=0){
                    return PARSE_ERROR;
                }
               
                if(buf.intialize(s*1024)){
                    return PARSE_ERROR;
                }
                return PARSE_OK;
            }
        }
        else if(!strcmp(argv[i], "-?")){
            return PARSE_ERROR;
        }

        return PARSE_UNKNOWN;
    }

    virtual void printHelp(const char* cmd){
        fprintf(stdout, "%s\n", cmd);
        fprintf(stdout, "\t[-? show this message.\n"); 
        fprintf(stdout, "\t[-i <fileName>] input file.\n"); 
        fprintf(stdout, "\t[-o <fileName>] output file.\n"); 
        fprintf(stdout, "\t[-m <size KB>] size(KB) of line buffer.\n"); 
    }

protected:
    FILE* in;
    FILE* out;

    Buffer buf;

private:
    int _parseArgs(int argc, const char* argv[]){
        for(int i=1; i<argc; ++i){
            if(parseArgs(argc, argv, i)!=PARSE_OK){
                printHelp(argv[0]);
                return 1;
            }
        }

        if(in == NULL){
            in = stdin;
        }

        if(out == NULL){
            out = stdout;
        }

        return 0;
    }
};

#endif
