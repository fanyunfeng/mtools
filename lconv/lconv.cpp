#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/IOApp.h"
#include "../lib/Conv.h"

class Lconv: public IOApp{
public:
    Lconv(){
        fromcode = NULL;
        tocode = NULL;
    }

    int prepare();
    void printHelp(const char* cmd);
    virtual int handleLine(Buffer& buf);
    virtual int parseArgs(int argc, const char* argv[], int& i);

private:
    const char* fromcode;
    const char* tocode;

    Conv conv;
    Buffer buf;
};

void Lconv::printHelp(const char* cmd){
    IOApp::printHelp(cmd);

    fprintf(stdout, "\t<-f <fromcode>>.\n"); 
    fprintf(stdout, "\t<-t <tocode>>.\n"); 
}

int Lconv::parseArgs(int argc, const char* argv[], int& i){
    int ret = IOApp::parseArgs(argc, argv, i);

    switch(ret){
    case PARSE_OK:
    case PARSE_ERROR:
        return ret;
    }

    if(!strcmp(argv[i], "-f")){
        if(i+1<argc){
            ++ i;

            fromcode = argv[i];
            return PARSE_OK;
        }
    }
    else if(!strcmp(argv[i], "-t")){
        if(i+1<argc){
            ++ i;
            tocode = argv[i];
            return PARSE_OK;
        }
    }
    return PARSE_UNKNOWN;
}

int Lconv::prepare(){
    conv.init(tocode, fromcode);

    int ret = 0;
    if((ret = IOApp::prepare())){
        return ret;
    }

    size_t s = IOApp::buf.size;

    if(s==0){
        s = 16 * 1024;
    }

    return buf.intialize(s);
}

int Lconv::handleLine(Buffer& line){
    buf.len = conv.convert(line.buf, line.len, buf.buf, buf.size);

    if(buf.len == (size_t)-1){
        return 1;
    }

    fwrite(buf.buf, buf.len, 1, out);
    return 0;
}

int main(int argc, const char* argv[]){
    Lconv conv;

    return conv.run(argc, argv);
}
