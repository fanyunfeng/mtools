#ifndef	_CRegex_HH_
#define	_CRegex_HH_ 0

#include <regex.h>
#include <sys/types.h>


class HtRegex 
{
public:
    CRegex(const char *str, int case_sensitive = 0);
    ~CRegex();

    int		match(const char *str, int nullmatch, int nullstr);

protected:
    void init(const char* str, int case_sensitive);
    regex_t		re;
};

#endif
