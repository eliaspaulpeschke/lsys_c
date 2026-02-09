#include "lsystem_parser.h"
#include "../util/util.h"
#include "lsystem.h"
#include "raylib.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char * file_to_mem(char * fpath){
    FILE *file = fopen(fpath, "r");
    struct stat sb;
    stat(fpath, &sb);
    char *content = malloc(sb.st_size + 1);
    if (content == NULL) return NULL;
    ulong r = fread(content, 1, sb.st_size, file);
    if (r < sb.st_size) {
        TraceLog(LOG_WARNING
                , "Warning in file_to_mem, fread read less than the file");
    }
    content[sb.st_size] = '\0';
    fclose(file);
    return content;
}

int line_list(char* result[], unsigned int line_num, char * text) {
    char * res = strtok(text, "\n");
    unsigned int i = 0;
    while((res != NULL) && (i < line_num)){
        result[i] = res;
        res = strtok(NULL, "\n");
        i++;
    }
    if (res != NULL) {
        TraceLog(LOG_ERROR
                , "Error in parse_lsys: more than %d lines in text \n", line_num);
        return(-1);
    }
    return i;
}

char * parse_axiom(char * line){
    char * start = strpbrk(line, ":");
    if (start == NULL) {
        TraceLog(LOG_WARNING
                , "Warning in parse_axiom: unnamed axiom \n");
    }
    start = trim(start + 1);
    return start;
}

typedef struct Prem_and_ctx {
    char *premise;
    char *lcont;
    char *rcont;
} Prem_and_ctx;

static const Prem_and_ctx err_prem_and_ctx = (Prem_and_ctx){NULL, NULL, NULL};

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
            TraceLog(LOG_WARNING, "Warning in parse_prem_and_ctx: a > b < c occurred in context part. Interpreting as a < b > cq \n");
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
        TraceLog(LOG_ERROR
                , "Error in parse_prem_and_ctx: malformed premise \" %s \" \n"
                , premise);
        return err_prem_and_ctx; 
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
        TraceLog( LOG_ERROR
                , "Error in parse_rule: no -> found \n");
        return err_rule;
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
                if (prem_and_ctx.premise == NULL) {
                    return err_rule; 
                }
                break;
            case RS_QUALIFIER:
                TraceLog(LOG_WARNING
                        , "Warning: qualifiers not yet implemented \n");
                break;
            default:
                TraceLog(LOG_ERROR
                       , "Error in parse_rule: unforseen state\n");
                return err_rule; 
        }
        res = strtok(NULL, ":");
        state++;
    }
    if (res != NULL || state < RS_PREMISE) {
        TraceLog(LOG_ERROR
                , "Error in parse_rule: to less or too many colons \n");
        return err_rule; 
    }

    return (Rule){ name
                 , prem_and_ctx.premise
                 , prem_and_ctx.lcont
                 , prem_and_ctx.rcont
                 , result 
                 }; 
}

Ruleset parse_ruleset(char * lines[], unsigned int line_num, char * free_me){
    Ruleset rules = {
          .ruleset = malloc(sizeof(Rule) * line_num)
        , .num_rules = line_num
        , .free_me = free_me
    };
    if (rules.ruleset == NULL) {
        TraceLog(LOG_ERROR
                , "Could not allocate space for ruleset");
        return err_ruleset;
    }
    for (unsigned int i = 0; i < line_num; i++){
        *(rules.ruleset + i) = parse_rule(lines[i]);
    }
    return rules;
}

Lsystem parse_lsystem(char * lines[], unsigned int line_num, bool with_axiom, char * free_me){
    char * axiom = NULL;
    unsigned int rule_num = line_num;
    if (with_axiom){
      axiom = parse_axiom(lines[0]);
      rule_num -= 1;
    }
    char * name = "not implemented";
    int start = 0;
    if (with_axiom) start = 1;
    Ruleset rules = parse_ruleset(lines + start,line_num - start, NULL);
    if (rules.num_rules == -1 || rules.ruleset == NULL){
        return err_lsystem;
    }
    return (Lsystem){.ruleset = rules, .name = name, .axiom = axiom,.free_me = free_me};
}

Lsystem lsystem_from_file(char * filename, bool with_axiom){
    char * data = file_to_mem(filename);
    if (data == NULL) return err_lsystem;
    char * lines[64];
    unsigned int line_num = line_list(lines, 64, data);
    return parse_lsystem(lines, line_num, with_axiom, data);
}

Lsystem lsystem_from_string(char * text, bool with_axiom){
    char * data = strdup(text);
    if (data == NULL) return err_lsystem;
    char * lines[64];
    unsigned int line_num = line_list(lines, 64, data);
    return parse_lsystem(lines, line_num, with_axiom, data);
}

Ruleset ruleset_from_string(char * text){
    char * data = strdup(text);
    if (data == NULL) return err_ruleset;
    char * lines[64];
    unsigned int line_num = line_list(lines, 64, data);
    return parse_ruleset(lines, line_num, data);
}


