#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LSYSTEM_H

typedef struct Rule {
    char *name;
    char *premise; 
    char *lcont;
    char *rcont;
    char *result;
    } Rule;

typedef struct Lsystem {
    uint nrules;
    char *name;
    char *axiom;
    Rule *ruleset;
} Lsystem;

Rule mkRule (char * premise, char * result) {
    char *the_res = malloc(strlen(result) + 1);
    Rule the_rule = {premise, the_res, NULL, NULL};
    strcpy(the_rule.result, result);
    return the_rule;
}

char * applyRules( Rule rules[], uint n_rules, char * input){
    uint output_len = strlen(input) << 1;
    uint output_pos = 0;
    char * the_output = malloc(output_len);
    for(uint i = 0; i < strlen(input); i++){
        char prem = input[i];
        bool copyflag = true;
        for (uint j = 0; j < n_rules; j++){
            Rule current_rule = rules[j];
            size_t reslen = strlen(current_rule.result);
            if (prem == *current_rule.premise) {
                while (output_len <= 
                        output_pos + reslen) {
                    output_len = output_len << 1;
                    the_output = realloc(the_output, output_len);
                }
                // copy with the \0 and add only one less to pos, 
                // subsequently overwriting the \0 
                memcpy(the_output + output_pos, current_rule.result, reslen + 1);
                output_pos += reslen;
                copyflag = false;

            }
        }
        if (copyflag) {
            if (output_pos < output_len) {
              the_output[output_pos] = prem;
              output_pos += 1;
            }
        }
    }
    return the_output;
}


void print_rule(Rule * rule, char * offset){
    printf("%sName: %s\n", offset, rule->name);
    printf("%s  %s < %s > %s -> %s \n", offset
                                   , rule->lcont
                                   , rule->premise
                                   , rule->rcont
                                   , rule->result
                                   );
}



