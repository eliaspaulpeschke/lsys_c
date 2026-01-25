#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Rule {
    char premise;
    uint result_len;
    char *result;
} Rule;

typedef struct Lstring {
    uint length;
    char *str;
} Lstring;

typedef struct Lsystem {
    uint nrules;
    Lstring init;
    Rule *ruleset;
} Lsystem;

Rule mkRule (char premise, uint result_len, char * result) {
    char *the_res = malloc((result_len) * sizeof(char));
    Rule the_rule = {premise, result_len, the_res};
    strcpy(the_rule.result, result);
    return the_rule;
}

Lstring applyRules( Rule rules[], uint n_rules, Lstring input){
    uint output_len = input.length << 1;
    uint output_pos = 0;
    char * the_output = malloc(output_len);
    for(uint i = 0; i < input.length; i++){
        char prem = input.str[i];
        bool copyflag = true;
        for (uint j = 0; j < n_rules; j++){
            Rule current_rule = rules[j];
            if (prem == current_rule.premise) {
                while (output_len <= 
                        output_pos + current_rule.result_len) {
                    output_len = output_len << 1;
                    the_output = realloc(the_output, output_len);
                }
                memcpy(the_output + output_pos, current_rule.result, current_rule.result_len - 1);
                output_pos += current_rule.result_len -1;
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
    /* THIS IS THE WRONG LENGTH */
    Lstring res = {output_len, the_output};
    return res;
}
