#ifndef LSYSTEM_H
#define LSYSTEM_H
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct Rule {
    char *name;
    char *premise; 
    char *lcont;
    char *rcont;
    char *result;
    } Rule;

typedef struct { 
    unsigned int num_rules;
    Rule *ruleset;
    char *free_me;
} Ruleset;

typedef struct Lsystem {
    char *name;
    char *axiom;
    Ruleset ruleset;
    char *free_me;
} Lsystem;

static const Rule err_rule = (Rule){NULL, NULL, NULL, NULL, NULL};
static const Ruleset err_ruleset = (Ruleset) {-1, NULL, NULL};
static const Lsystem err_lsystem = (Lsystem) {NULL, NULL, (Ruleset) {-1, NULL, NULL} ,NULL};

void free_ruleset(Ruleset rules);

void free_lsystem(Lsystem sys);

char * applyRules( Ruleset ruleset, char * input, bool freeInput);

char * str_rule(Rule * rule, char * offset);

void print_rule(Rule * rule, char * offset);

#endif

