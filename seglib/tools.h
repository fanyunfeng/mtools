
char* strEndl(char* str){
    char* p = str;
    while(*p){
        switch(*p){
        case '\r':
        case '\n':
            *p = 0;
            return p;
        default:
            ++ p;
            break;
        }
    }

    return str;
}
