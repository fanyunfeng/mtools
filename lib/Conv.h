#ifndef _H_CONF_HH_
#define _H_CONF_HH_


#include <iconv.h>
class Conv{
public:
    Conv(){
        cd = (iconv_t)-1;
    }

    Conv(const char* tocode, const char* fromcode){
        init(tocode, fromcode);
    }

    ~Conv(){
        if(cd != (iconv_t) -1){
            iconv_close(cd);
            cd = (iconv_t) -1;
        }
    }

    int init(const char* tocode, const char* fromcode){
        cd = iconv_open(tocode, fromcode);

        if(cd != (iconv_t) -1){
            return 0;
        }

        return 1;
    }

    int reset(){
        return 0;
    }

    size_t convert(char* in, size_t len, char* out, size_t size){
        size_t _size = size;
        size_t ret = iconv(cd, &in, &len, &out, &size);

        if(ret == (size_t) -1){
            return ret;
        }

        return _size - size;
    }
private:
    iconv_t cd;
};

#endif
