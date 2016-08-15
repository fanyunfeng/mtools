#ifndef	_CRegex_HH_
#define	_CRegex_HH_ 0

#include <regex.h>
#include <sys/types.h>
#include <stdio.h>


class CRegex 
{
public:
    CRegex(const char *str, int case_sensitive = 0){
        init(str, case_sensitive);
    }

    ~CRegex(){
        regfree(&re);
    }

    int match(const char *str, int nullmatch, int nullstr){
        return !regexec(&re, str, (size_t) 0, NULL, 0);
    }

protected:
    void init(const char* str, int case_sensitive){
        regcomp(&re, str, case_sensitive ? REG_EXTENDED : (REG_EXTENDED|REG_ICASE));
    }

    regex_t		re;
};


#endif
