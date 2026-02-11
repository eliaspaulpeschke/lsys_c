#ifndef LSYSTEM2_H
#define LSYSTEM2_H
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define CONTEXT_SIZE 3

typedef struct {
    char * str;
    unsigned int len;
    unsigned int pos;
} ParserStr;

typedef enum {
     LPAYLOAD_dbl
   , LPAYLOAD_letter
   , LPAYLOAD_string
   , LPAYLOAD_unary_op_dbl
   , LPAYLOAD_binary_op_dbl
   , LPAYLOAD_unary_op_bool
   , LPAYLOAD_binary_op_dbl_bool
   , LPAYLOAD_binary_op_bool_bool
   , LPAYLOAD_empty
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
    LAstNode ** calculations;
} LResultWord;

double lop_unary_plus(double x);
double lop_unary_minus(double x);
double lop_binary_plus(double x, double y);
double lop_binary_minus(double x, double y);
double lop_multiply(double x, double y);
double lop_divide(double x, double y);

typedef struct LRule {
    char * name;
    LRuleWord premise;
    unsigned int l_context_size;
    LRuleWord l_context[CONTEXT_SIZE];
    LRuleWord r_context[CONTEXT_SIZE];
    LAstNode * qualifier;
    unsigned int num_result_words;
    LResultWord * result;
} LRule;

typedef enum {
    PARSE_DATA_string
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

void print_LAst( LAstNode * node, char * offset); 

ParseData * create_parse_data(ParseDataType type);

LAstNode * create_node(LPayload payload);
void node_add_child(LAstNode * node, LAstNode * child);
void node_set_parent(LAstNode * node, LAstNode * parent);

void init_ast_buffer();
void reset_ast_buffer();
void free_ast_buffer();

int parser_str_getch(ParserStr * str);
ParserStr * mk_parser_str(char * str);
#endif

