#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "tools.h"

#include <map>

typedef std::map<unsigned char, FILE*> FileMap;
std::map<unsigned char, FILE*> files;

int destPathLength;
char* destPath;

FILE* openNewFile(unsigned char ch){
    char* buf = (char*) malloc(destPathLength+10);

    snprintf(buf, destPathLength+10, "%s/%02X.txt", destPath, ch);

    FILE* file = fopen(buf, "w+");

    if(file!=NULL){
        files[ch] = file;
    }

    return file;
}
    
FILE* getFile(unsigned char ch){
    FileMap::iterator it = files.find(ch);

    if(it == files.end()){
        return openNewFile(ch);
    }

    return it->second;
}

void closeAll(){
    FileMap::iterator it = files.begin();
    for(; it!=files.end(); ++it){
        FILE* f = it->second;

        fclose(f);
    }

    files.clear();
}

int saveWord(char* word){
    FILE* file = getFile(word[0]);
    if(file != NULL){
        fprintf(file, "%s\n", word);
        return 0;
    }

    return 1;
}

int split(char* file, char* path){
    char* line;
    size_t len = 0;
    size_t read ;
    char* end;

    destPath = path;
    destPathLength = strlen(path);

    FILE* fp = fopen(file, "r");

    if(file == NULL){
        return 1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        end = strEndl(line);

        if(end == line){
            continue;
        }

        saveWord(line); 
    }

    free(line);
    closeAll();

    return 0;
}

int main(int argc, char* argv[]){
    return split(argv[1], argv[2]);
}
