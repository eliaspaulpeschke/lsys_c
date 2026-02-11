#include "lsystem2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSYSTEM_AST_BUFFER_SIZE 1024

static char * ast_buffer; 
static unsigned int ast_buffer_pos;

void init_ast_buffer(){
    ast_buffer = malloc(LSYSTEM_AST_BUFFER_SIZE);
}

void reset_ast_buffer(){
    memset(ast_buffer, 0, LSYSTEM_AST_BUFFER_SIZE);
    ast_buffer_pos = 0;
}

void free_ast_buffer(){
    free(ast_buffer);
}
void print_LAst( LAstNode * node, char * offset){
    if (node == NULL) return;
    char new_offset[strlen(offset)+4];
    memset(new_offset, ' ', strlen(offset)+4);
    new_offset[strlen(offset)+3] = '\0';
    printf("%s node\n", offset);
    switch (node->payload.type) {
        case LPAYLOAD_dbl:
            printf("%s double: %f\n", new_offset, node->payload.dbl_val);
            break;
        case LPAYLOAD_bool:
            printf("%s bool: %b\n", new_offset, node->payload.bool_val);
            break;
        case LPAYLOAD_letter:
            printf("%s letter: %c\n", new_offset, node->payload.letter_val);
            break;
        case LPAYLOAD_string:
            printf("%s string: %s\n", new_offset, node->payload.string_val);
            break;
        case LPAYLOAD_unary_op_dbl:
            printf("%s unary double: %p\n", new_offset, node->payload.unary_op_dbl_val);
            break;
        case LPAYLOAD_binary_op_dbl:
            printf("%s binary double: %p\n", new_offset, node->payload.binary_op_dbl_val);
            break;
        case LPAYLOAD_unary_op_bool:
            printf("%s unary bool: %p\n", new_offset, node->payload.unary_op_bool_val);
            break;
         case LPAYLOAD_binary_op_dbl_bool:
            printf("%s binary double bool: %p\n", new_offset, node->payload.binary_op_dbl_bool_val);
            break;
        case LPAYLOAD_binary_op_bool_bool:
            printf("%s binary bool bool: %p\n", new_offset, node->payload.binary_op_bool_bool_val);
            break;
        default:
            break;
    }
    for(int i = 0; i < node->child_count; i++){
        print_LAst(node->child[i], new_offset);
    }
}

LAstNode * mk_node(LPayload payload){
//    ast_buffer_pos += sizeof(LAstNode);
    LAstNode * node = malloc(sizeof(LAstNode));
    *node = (LAstNode){
        .parent = NULL
      , .child_count = 0
      , .child = NULL
      , .payload = payload  
    };
    return node;
}

void node_set_parent(LAstNode * node, LAstNode * parent){
    node->parent = parent;
}
void node_add_child(LAstNode * node, LAstNode * child){
    if (child->parent != NULL) return;
    if (node->child_count >= 10) return;
    node->child[node->child_count] = child;
    node->child_count+=1;
    node_set_parent(child, node);
}

int parser_str_getch(ParserStr * str){
    if (str->pos < str->len){
        int val = str->str[str->pos];
        str->pos += 1;
        return val;
    }
    return -1;
}

ParserStr * mk_parser_str(char * str){
    ParserStr * pst = malloc(sizeof(ParserStr));
    *pst = (ParserStr){
          .str = strdup(str)
        , .pos = 0
        , .len = strlen(str)
    };
    return pst;
}

ParseData * mk_parse_data(ParseDataType type){
    ParseData * pd = malloc(sizeof(ParseData));
    *pd = (ParseData) {.type = type, 0};
    return pd;
};

bool is_parse_data_empty(ParseData *pd){
    if (pd == NULL) return true;
    if (pd->type == PARSE_DATA_empty) return true;  
    if (pd->list_length == 0) return true;
    if (pd->parse_data_val == NULL) return true;
    return false;
}

double lop_unary_plus(double x){ return (+x); }
double lop_unary_minus(double x){ return (-x); }
double lop_binary_plus(double x, double y){ return (x + y); }
double lop_binary_minus(double x, double y){ return (x - y); }
double lop_multiply(double x, double y){ return (x * y); }
double lop_divide(double x, double y){ return (x / y); }

bool lop_eq(double l, double r){  return (l == r); }
bool lop_lt(double l, double r){  return (l  < r); }
bool lop_gt(double l, double r){  return (l  > r); }
bool lop_leq(double l, double r){ return (l <= r); }
bool lop_geq(double l, double r){ return (l >= r); }
bool lop_neq(double l, double r){ return (l != r); }

bool lop_not(bool x){ return (!x); }
bool lop_and(bool l, bool r){ return (l && r); }
bool lop_or(bool l, bool r){ return (l || r); }


