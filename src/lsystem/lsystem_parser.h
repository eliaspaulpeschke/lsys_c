#ifndef LSYSTEM_PARSER_H
#define LSYSTEM_PARSER_H
#include <unistd.h>
#include <sys/stat.h>
#include "lsystem.h"

char * file_to_mem(char * fpath);
int line_list(char* result[], unsigned int line_num, char * text);
char * parse_axiom(char * line);
Rule parse_rule(char * line);
Ruleset parse_ruleset(char * lines[], unsigned int line_num, char * free_me);
Lsystem parse_lsystem(char * lines[], unsigned int line_num, bool with_axiom, char * free_me);
Lsystem lsystem_from_file(char * filename, bool with_axiom);
Lsystem lsystem_from_string(char * text, bool with_axiom);
Ruleset ruleset_from_string(char * text);

#endif
