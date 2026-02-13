#ifndef LSYSTEM_H
#define LSYSTEM_H
#include <limits.h>
#include <stdbool.h>

typedef struct {
    char * str;
    unsigned int len;
    unsigned int pos;
} ParserStr;

typedef enum {
     LPAYLOAD_empty = 0
   , LPAYLOAD_dbl
   , LPAYLOAD_letter
   , LPAYLOAD_bool
   , LPAYLOAD_string
   , LPAYLOAD_unary_op_dbl
   , LPAYLOAD_binary_op_dbl
   , LPAYLOAD_unary_op_bool
   , LPAYLOAD_binary_op_dbl_bool
   , LPAYLOAD_binary_op_bool_bool
}LPayloadType;

typedef double(*unary_op_dbl)(double);
typedef double(*binary_op_dbl)(double, double);
typedef bool(*unary_op_bool)(bool);
typedef bool(*binary_op_dbl_bool)(double, double);
typedef bool(*binary_op_bool_bool)(bool,bool);

typedef struct {
    LPayloadType type;
    union {
        double dbl_val;
        char letter_val;
        bool bool_val;
        char * string_val;
        unary_op_dbl unary_op_dbl_val;
        binary_op_dbl binary_op_dbl_val;
        unary_op_bool unary_op_bool_val;
        binary_op_dbl_bool binary_op_dbl_bool_val;
        binary_op_bool_bool binary_op_bool_bool_val;
    };
}LPayload;

typedef struct LAstNode {
    struct LAstNode * parent;
    unsigned int child_count;
    struct LAstNode * child[10];
    LPayload payload;
} LAstNode;

typedef struct {
    char letter;
    unsigned int value_idx;
} LBinding;

typedef struct {
    char name;
    unsigned int num_values;
    double * values;
} LWord;

typedef struct {
    char name;
    unsigned int num_bindings;
    char * bindings;
} LRuleWord;

typedef struct {
    char name;
    unsigned int num_calculations;
    LAstNode * calculations;
} LResultWord;

typedef struct {
    unsigned int length;
    unsigned int capacity;
    LWord * content;
}LString;

double lop_unary_plus(double x);
double lop_unary_minus(double x);
double lop_binary_plus(double x, double y);
double lop_binary_minus(double x, double y);
double lop_multiply(double x, double y);
double lop_divide(double x, double y);

bool lop_eq(double l, double r);
bool lop_lt(double l, double r);
bool lop_gt(double l, double r);
bool lop_leq(double l, double r);
bool lop_geq(double l, double r);
bool lop_neq(double l, double r);

bool lop_not(bool x);
bool lop_and(bool l, bool r);
bool lop_or(bool l, bool r);

typedef struct LRule {
    char * name;
    LRuleWord premise;
    unsigned int l_context_size;
    unsigned int r_context_size;
    LRuleWord * l_context;
    LRuleWord * r_context;
    LAstNode * qualifier;
    unsigned int num_result_words;
    LResultWord * result;
} LRule;

typedef enum {
    PARSE_DATA_empty = 0
  , PARSE_DATA_string
  , PARSE_DATA_LRuleWord
  , PARSE_DATA_LAstNode
  , PARSE_DATA_LResultWord
  , PARSE_DATA_LRule
  , PARSE_DATA_ParseData
} ParseDataType;

typedef struct ParseData{
    ParseDataType type;
    unsigned int list_length;
    union {
        char * string_val;
        LRuleWord * lruleword_val;
        LAstNode * lastnode_val;
        LResultWord * lresultword_val;
        LRule * lrule_val;
        struct ParseData * parse_data_val;
    };
} ParseData;

typedef struct LRuleset {
    unsigned int num_rules;
    LRule * rules;
} LRuleset;

void print_LAst( LAstNode * node, char * offset); 
void print_pd( ParseData * pd, char * offset);
ParseData * mk_parse_data(ParseDataType type);
bool is_parse_data_empty(ParseData * pd);
LAstNode * mk_node(LPayload payload);

void node_add_child(LAstNode * node, LAstNode * child);
void node_set_parent(LAstNode * node, LAstNode * parent);

void init_ast_buffer();
void reset_ast_buffer();
void free_ast_buffer();

int parser_str_getch(ParserStr * str);
ParserStr * mk_parser_str(char * str);

LString * apply_rules(LRuleset rules, LString * input);
#endif

