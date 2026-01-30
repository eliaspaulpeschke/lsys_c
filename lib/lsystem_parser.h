#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef LSYSTEM_H
#include "lsystem.h"
#endif

#define LSYSTEM_PARSER_H

char * file_to_mem(char * fpath){
    FILE *file = fopen("test", "r");
    struct stat sb;
    stat("test", &sb);
    char *content = malloc(sb.st_size + 1);
    if (content == NULL) return NULL;
    ulong r = fread(content, 1, sb.st_size, file);
    if (r < sb.st_size) {
        printf("Warning in file_to_mem, fread read less than the file");
    }
    content[sb.st_size] = '\0';
    fclose(file);
    return content;
}

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

int line_list(char* result[],  uint line_num, char * text) {
    char * res = strtok(text, "\n");
    uint i = 0;
    while((res != NULL) && (i < line_num)){
        result[i] = res;
        res = strtok(NULL, "\n");
        i++;
    }
    if (res != NULL) {
        printf("Error in parse_lsys: more than %d lines in text \n", line_num);
        return(-1);
    }
    return i;
}


char * parse_axiom(char * line){
    char * start = strpbrk(line, ":");
    if (start == NULL) {
        printf("Warning in parse_axiom: unnamed axiom \n");
    }
    start = trim(start + 1);
    return start;
}

typedef struct Prem_and_ctx {
    char *premise;
    char *lcont;
    char *rcont;
} Prem_and_ctx;

Prem_and_ctx parse_prem_and_ctx (char * line) {
    char * lt = strstr(line, "<");
    char * gt = strstr(line, ">");
    char * premise;
    if (lt == NULL && gt == NULL) {
        premise = trim(line);
    } else if (lt == NULL) {
        *gt = '\0';
        gt = trim(gt + 1);
        premise = trim(line);
    }  else if (gt == NULL) {
        *lt = '\0';
        premise = trim(lt + 1);
        lt = trim(line);
    } else {
        if (lt > gt) {
            printf("Warning in parse_prem_and_ctx: a > b < c occurred in context part. Interpreting as a < b > cq \n");
            char * x = lt;
            lt = gt;
            gt = x;
        }
        *lt = '\0';
        *gt = '\0';
        premise = trim(lt + 1);
        lt = trim(line);
        gt = trim(gt + 1);
    }
    if (strlen(premise) != 1) {
        printf("Error in parse_prem_and_ctx: malformed premise \" %s \" \n", premise);
    }
    return (Prem_and_ctx){premise, lt, gt};
}

enum rule_state {
      RS_NAME
    , RS_PREMISE
    , RS_QUALIFIER
    , RS_FIN
};

Rule parse_rule(char * line){
    Prem_and_ctx prem_and_ctx;
    char * result;
    char * name;
    char * arrow = strstr(line, "->");
    if (arrow == NULL){
        printf("Error in parse_rule: no -> found \n");
    }
    result = trim(arrow + 2);
    *arrow = '\0';
    char * res = strtok(line, ":");
    enum rule_state state = RS_NAME;
    while (res != NULL && state < RS_FIN) {
        switch (state){
            case RS_NAME:
                name = trim(res);
                break;
            case RS_PREMISE:
                prem_and_ctx = parse_prem_and_ctx(trim(res));
                break;
            case RS_QUALIFIER:
                printf("Warning: qualifiers not yet implemented \n");
                break;
            default:
                printf("Error in parse_rule: unforseen state\n");
        }
        res = strtok(NULL, ":");
        state++;
    }
    if (res != NULL || state < RS_PREMISE) {
        printf("Error in parse_rule: to less or too many colons \n");
    }
//    return (Rule){NULL, NULL, NULL, NULL, NULL};

    return (Rule){ name
                 , prem_and_ctx.premise
                 , prem_and_ctx.lcont
                 , prem_and_ctx.rcont
                 , result 
                 }; 
}

Lsystem parse_lsystem(char * lines[], uint line_num, bool with_axiom, char * free_me){
    char * axiom = NULL;
    if (with_axiom){
      axiom = parse_axiom(lines[0]);
      line_num -= 1;
    }
    char * name = "not implemented";
    Rule * ruleset = malloc(sizeof(Rule) * (line_num -1)); 
    if (ruleset == NULL) return (Lsystem){-1, NULL, NULL, NULL, free_me};
    int start = 0;
    if (with_axiom) start = 1;
    for (int i = start; i < line_num; i++){
      ruleset[i - 1] = parse_rule(lines[i]);
    }
    return (Lsystem){line_num -1, name, axiom, ruleset, free_me};
}

Lsystem lsystem_from_file(char * filename, bool with_axiom){
    char * data = file_to_mem(filename);
    if (data == NULL) return (Lsystem){-1, NULL, NULL, NULL, NULL};
    char * lines[64];
    uint line_num = line_list(lines, 64, data);
    printf("line num: %d", line_num);
    return parse_lsystem(lines, line_num, with_axiom, data);
}

Lsystem lsystem_from_string(char * text, bool with_axiom){
    char * data = strdup(text);
    if (data == NULL) return (Lsystem){-1, NULL, NULL, NULL, NULL};
    char * lines[64];
    uint line_num = line_list(lines, 64, data);
    printf("line num: %d", line_num);
    return parse_lsystem(lines, line_num, with_axiom, data);
}

void free_lsystem(Lsystem sys){
    if (sys.free_me != NULL){
        free(sys.free_me);
    }
}






