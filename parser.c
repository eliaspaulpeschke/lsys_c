#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
    char * cnt;
    uint size;
} Filecontent;

Filecontent file_to_mem(char * fpath){
    FILE *file = fopen("test", "r");
    struct stat sb;
    stat("test", &sb);
    char *content = malloc(sb.st_size);
    fread(content, 1, sb.st_size, file);
    fclose(file);
    Filecontent res = {content, sb.st_size};
    return res;
}



