#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/IOApp.h"

class FieldInfo{
public:
    const char* prefix;
    int         len;
};

class Ncut: public IOApp{
public:
    Ncut(){
        size = 0;
        fields = NULL;
        withFieldName = 0;
        buf = (char*) malloc(8192);
    }

    int prepare();
    void printHelp(const char* cmd);
    virtual int handleLine(Buffer& buf);
    virtual int parseArgs(int argc, const char* argv[], int& i);

private:
    int size;
    FieldInfo* fields;

    int withFieldName;
    char* buf;

private:
    const char* findValueStart(const char* str, const FieldInfo& info, const char*& end);
    const char* findValueEnd(const char* str);
    const char* findValueStart(const char* str);
};

void Ncut::printHelp(const char* cmd){
    IOApp::printHelp(cmd);

    fprintf(stdout, "\t<-n <fieldname>>.\n"); 
    fprintf(stdout, "\t[--with-field-name]\n"); 
}

int Ncut::parseArgs(int argc, const char* argv[], int& i){
    int ret = IOApp::parseArgs(argc, argv, i);

    switch(ret){
    case PARSE_OK:
    case PARSE_ERROR:
        return ret;
    }

    if(fields==NULL){
        fields = new FieldInfo[argc];
    }

    if(!strcmp(argv[i], "-n")){
        if(i+1<argc){
            ++ i;
           
            fields[size].prefix = argv[i];
            fields[size].len = strlen(argv[i]);

            size ++;
            return PARSE_OK;
        }
    }
    else if(!strcmp(argv[i], "--with-field-name")){
        withFieldName = 1;
        return PARSE_OK;
    }
    return PARSE_UNKNOWN;
}

int Ncut::prepare(){
    return 0;
}

const char* Ncut::findValueStart(const char*str){
    while(*str){
        switch(*str){
        case ' ':
        case ':':
        case '=':
            str ++;
            break;
        default:
            return str;
        }
    }

    return NULL;
}

const char* Ncut::findValueStart(const char* str, const FieldInfo& info, const char*& end){
    const char* start = NULL;

    start = strstr(str, info.prefix);

    if(start == NULL){
        return NULL;
    }

    end = start + info.len;

    start = findValueStart(end);

    if(start == NULL){
        return NULL;
    }
   
    end = findValueEnd(start);

    return start;
}

const char* Ncut::findValueEnd(const char* str){
    while(*str){
        switch(*str){
        case ' ':
        case ',':
        case ';':
        case '\r':
        case '\n':
        case '\t':
            return str;
        default:
            str ++;
            break;
        }
    }

    return str;
}

int Ncut::handleLine(Buffer& line){
    const char* start=NULL;
    const char* end=NULL;
    char* p;
    int len=0;

    p = buf;

    for(int i=0; i<size; ++i){
        start = findValueStart(line.buf, fields[i], end);

        if(start){
            if(p!=buf){
                *p = ' ';
                ++p;
            }

            if(withFieldName){
                strncpy(p, fields[i].prefix, fields[i].len);
                p += fields[i].len;

                *p = ':';
                ++p;
            }

            len = end - start;

            strncpy(p, start, len);
            p += len;
        }
    }

    if(p!=buf){
        *p = '\n';
        ++p;

        fwrite(buf, p-buf, 1, out);
    }
    return 0;
}

int main(int argc, const char* argv[]){
    Ncut app;

    return app.run(argc, argv);
}
