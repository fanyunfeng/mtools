#include "CRegex.h"

/*

CRegex::CRegex(const char *str, int case_sensitive)
{
    init(str, case_sensitive);
}

CRegex::~CRegex()
{
    regfree(&re);
}


void CRegex::init(const char * str, int case_sensitive)
{
    regcomp(&re, str, case_sensitive ? REG_EXTENDED : (REG_EXTENDED|REG_ICASE));
}

int CRegex::match(const char * str, int nullpattern, int nullstr)
{
    int	rval;

    rval = regexec(&re, str, (size_t) 0, NULL, 0);
    return !rval;
}

int CRegex::replace(const char * str,const char * Pattern, const char * replacestr ,char * result)
{
    size_t pos=0,len=strlen(replacestr);
    regmatch_t match;
    const char * t = str;
    result [0] = 0;
    set(Pattern);
    if (compiled == 0) return -1;
    if (str == NULL) return -1;
    if (strlen(str) <= 0) return -1;
    while(regexec(&re,t,1,&match,0)==0)
    {
        strncpy(result+pos,t,match.rm_so);//first copy the string that doesn't match
        pos+=match.rm_so;//change pos
        strncpy(result+pos,replacestr,len);//then replace 
        pos+=len;
        t+=match.rm_eo;//for another match
    }
    strncpy(result+pos,t,strlen(t));//don't forget the last unmatch string
    pos +=strlen(t);
    result[pos] = 0;
    return 0;
}
*/
