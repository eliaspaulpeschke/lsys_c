#include "lsystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

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

char * str_ruleword(LRuleWord * rw){
    int len = 1 + rw->num_bindings * 3 + 2 + 1;
    char * out = malloc(len);
    memset(out, '\0', len);
    out[0] = rw->name;
    unsigned int out_pos = 1;
    if (rw->num_bindings > 0){
        out[out_pos] = '(';
        out_pos++;
        for (int i = 0; i < rw->num_bindings; i++){
            out[out_pos] = rw->bindings[i];
            out_pos++;
            if(i < rw->num_bindings -1){
                out[out_pos] = ',';
                out_pos++;
                out[out_pos] = ' ';
                out_pos++;
            }
            
        }
        out[out_pos] = ')';
        out_pos++;
    }
    out[out_pos] = '\0';
    return out;
}

void print_pd(ParseData * pd, char * offset){
    if (pd == NULL) {
        printf("Parse Data is NULL");
    };
    char new_offset[strlen(offset)+4];
    memset(new_offset, ' ', strlen(offset)+4);
    new_offset[strlen(offset)+3] = '\0';
    char new_offset2[strlen(new_offset)+4];
    memset(new_offset2, ' '
          , strlen(new_offset)+4);
    new_offset2[strlen(new_offset)+3] = '\0';
    printf("%s ParseData of length %d:\n", offset, pd->list_length);

    if (pd->list_length < 1) return;
    switch (pd->type){
        case PARSE_DATA_empty:
            printf("%s empty\n", new_offset);
            break;
        case PARSE_DATA_string:
            printf("%s string: %s\n", new_offset, pd->string_val);
            break;
        case PARSE_DATA_LRuleWord:
            printf("%s LRuleWord: %s\n"
                  , new_offset, pd->string_val);
            for (int i = 0; i < pd->list_length; i++){
                printf("%s Word %d: %s\n"
                      , new_offset, i
                      , str_ruleword(pd->lruleword_val + i));
            }
            break;
        case PARSE_DATA_LResultWord:
            printf("%s LResultWord: \n"
                  , new_offset);
            for (int i = 0; i < pd->list_length; i++){
                LResultWord rw = pd->lresultword_val[i];
                printf("%s Word %d: %c\n"
                      , new_offset, i, rw.name);
                for(int j = 0; j < rw.num_calculations; j++){
                    printf("%s Calculation %d: \n"
                          , new_offset2, j);
                    print_LAst(rw.calculations + j, new_offset2);
                }
            }
            break;
        case PARSE_DATA_LAstNode:
            printf("%s LAstNode: \n"
                  , new_offset );
            for (int i = 0; i < pd->list_length; i++){
                print_LAst(pd->lastnode_val + i, new_offset);
            }
            break;
        case PARSE_DATA_LRule:
            printf("%s LRules: \n", new_offset);
       
            for (int i = 0; i < pd->list_length; i++){
                LRule r = pd->lrule_val[i];
                if (r.name != NULL) printf("%s Name: %s\n", new_offset, r.name);
                if (r.l_context != NULL) {
                    printf("%s Left Context: \n", new_offset);
                    for (int i = 0; i < r.l_context_size; i++){
                        printf("%s Word %d: %s\n"
                              , new_offset2, i
                              , str_ruleword(r.l_context + i));
                    }
                }
                printf("%s Premise: %s\n", new_offset, str_ruleword(&r.premise));
                if (r.r_context != NULL) {
                    printf("%s Right Context: \n", new_offset);
                    for (int i = 0; i < r.r_context_size; i++){
                        printf("%s Word %d: %s\n"
                              , new_offset2, i
                              , str_ruleword(r.r_context + i));
                    }
                }
                if (r.qualifier != NULL) {
                    printf("%s Qualifier: \n", new_offset);
                    print_LAst(r.qualifier, new_offset2);
                }
                printf("%s Result: \n", new_offset);
                for (int i = 0; i < r.num_result_words; i++){
                    LResultWord rw = r.result[i];
                    printf("%s Word %d: %c\n"
                          , new_offset2, i, rw.name);
                    for(int j = 0; j < rw.num_calculations; j++){
                        printf("%s Calculation %d: \n"
                              , new_offset2, j);
                        print_LAst(rw.calculations + j, new_offset2);
                    }
                }
            }
            break;
        default:
            break;
    }
}

void print_LAst( LAstNode * node, char * offset){
    if (node == NULL) return;
    char new_offset[strlen(offset)+4];
    memset(new_offset, ' ', strlen(offset)+4);
    new_offset[strlen(offset)+3] = '\0';
    printf("%s LAstNode\n", offset);
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

unsigned int char2idx(char c){
    return (unsigned int)c - 97;
}

typedef struct {
    bool is_set[26];
    double value[26];
} BindingValueList;

bool check_letterwise_apply(LRule rule, LString * input, unsigned int idx){
    if (rule.l_context_size < idx) return false;
    if (idx + rule.r_context_size >= input->length) return false;
    LRuleWord p = rule.premise;
    LWord w = input->content[idx];
    if (p.name != w.name) return false;
    for (unsigned int i = 0; i < rule.l_context_size; i++){
        LRuleWord lc = rule.l_context[i];
        unsigned int word_idx = idx - (rule.l_context_size - i);
        LWord w = input->content[word_idx]; 
        if (w.name != lc.name) return false;
    }
    for (unsigned int i = 0; i < rule.r_context_size; i++){
        LRuleWord rc = rule.r_context[i];
        unsigned int word_idx = idx + i + 1;
        LWord w = input->content[word_idx]; 
        if (w.name != rc.name) return false;
    }
    return true;
}

BindingValueList evaluate_words_bindings(BindingValueList b, LRuleWord rw, LWord w){
    unsigned int num_bindings = rw.num_bindings;
    if (w.num_values < num_bindings) {
        TraceLog(LOG_WARNING, "Word has less values than rule has Bindings!"); 
        num_bindings = w.num_values;
    }
    for (int i = 0; i < num_bindings; i ++){
        unsigned int binding_idx = char2idx(rw.bindings[i]);
        if (b.is_set[binding_idx]) {
            TraceLog(LOG_WARNING, "Binding is already set!"); 
            break; 
        }
        b.is_set[binding_idx] = true;
        b.value[binding_idx] = w.values[i];
    }
    return b;
}

BindingValueList evaluate_bindings(LRule rule, LString * input, unsigned int idx){
    BindingValueList b = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
    if (rule.l_context_size < idx) return b;
    if (idx + rule.r_context_size >= input->length) return b;
    for (unsigned int i = 0; i < rule.l_context_size; i++){
        LRuleWord lc = rule.l_context[i];
        unsigned int word_idx = idx - (rule.l_context_size - i);
        LWord w = input->content[word_idx]; 
        b = evaluate_words_bindings(b, lc, w);
    }
    for (unsigned int i = 0; i < rule.r_context_size; i++){
        LRuleWord rc = rule.r_context[i];
        unsigned int word_idx = idx + i + 1;
        LWord w = input->content[word_idx]; 
        b = evaluate_words_bindings(b, rc, w);
    }
    LRuleWord p = rule.premise;
    LWord w = input->content[idx];
    b = evaluate_words_bindings(b, p, w);
    return b;
}

typedef enum{
      EVALUATION_RESULT_BOOL
    , EVALUATION_RESULT_DBL
    , EVALUATION_RESULT_STRING
    , EVALUATION_RESULT_EMPTY
}EVALUATION_RES_TYPE;

typedef struct{
    EVALUATION_RES_TYPE type;
    union{
        bool bool_val;
        double dbl_val;
        char * string_val;
    };
}EvaluationResult;

EvaluationResult evaluate_ast(BindingValueList bindings, LAstNode node){
    unsigned int b_idx;
    EvaluationResult tmp1;
    EvaluationResult tmp2;
    double dbl_res;
    bool bool_res;
    EvaluationResult empty = (EvaluationResult){EVALUATION_RESULT_EMPTY};
    switch (node.payload.type) {
        case LPAYLOAD_empty:
            return empty;
        case LPAYLOAD_dbl:
            return (EvaluationResult){EVALUATION_RESULT_DBL, node.payload.dbl_val};
        case LPAYLOAD_string:
            return (EvaluationResult){EVALUATION_RESULT_STRING, node.payload.string_val};
        case LPAYLOAD_letter: 
            b_idx = char2idx(node.payload.letter_val);
            if (bindings.is_set[b_idx]){
                return (EvaluationResult){EVALUATION_RESULT_DBL, bindings.value[b_idx]};
            } else {
                return (EvaluationResult){EVALUATION_RESULT_EMPTY};
            }
        case LPAYLOAD_bool:
            return (EvaluationResult){EVALUATION_RESULT_BOOL, node.payload.bool_val};
        case LPAYLOAD_unary_op_dbl:
            if (node.child_count < 1) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_DBL) return empty;
            dbl_res = node.payload.unary_op_dbl_val(tmp1.dbl_val);
            return (EvaluationResult){EVALUATION_RESULT_DBL, dbl_res};
        case LPAYLOAD_binary_op_dbl:
            if (node.child_count < 2) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_DBL) return empty;
            tmp2 = evaluate_ast(bindings, *node.child[1]);
            if (tmp2.type != EVALUATION_RESULT_DBL) return empty;
            dbl_res = node.payload.binary_op_dbl_val(tmp1.dbl_val, tmp2.dbl_val);
            return (EvaluationResult){EVALUATION_RESULT_DBL, dbl_res};
        case LPAYLOAD_unary_op_bool:
            if (node.child_count < 1) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_BOOL) return empty;
            bool_res = node.payload.unary_op_bool_val(tmp1.bool_val);
            return (EvaluationResult){EVALUATION_RESULT_BOOL, bool_res};
        case LPAYLOAD_binary_op_dbl_bool:
            if (node.child_count < 2) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_DBL) return empty;
            tmp2 = evaluate_ast(bindings, *node.child[1]);
            if (tmp2.type != EVALUATION_RESULT_DBL) return empty;
            return (EvaluationResult){EVALUATION_RESULT_BOOL, bool_res};
        case LPAYLOAD_binary_op_bool_bool:
            if (node.child_count < 2) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_BOOL) return empty;
            tmp2 = evaluate_ast(bindings, *node.child[1]);
            if (tmp2.type != EVALUATION_RESULT_BOOL) return empty;
            bool_res = node.payload.binary_op_bool_bool_val(tmp1.bool_val, tmp2.bool_val);
            return (EvaluationResult){EVALUATION_RESULT_BOOL, bool_res};
    }
}

bool does_rule_apply(LRule rule, LString * input, unsigned int idx){
    if (rule.l_context_size < idx) return false;
    LWord word = input->content[idx];
    if (word.name != rule.premise.name) return false;
    if (!check_letterwise_apply(rule, input, idx)) return false;
    if (rule.qualifier == NULL) return true;
    BindingValueList bindings = evaluate_bindings(rule, input, idx);
    EvaluationResult res = evaluate_ast(bindings, *rule.qualifier);
    if (res.type != EVALUATION_RESULT_BOOL) return false;
    return res.bool_val;
}

void append_result(LString * input, LRule rule, LString * output, unsigned int idx){
    BindingValueList bindings = evaluate_bindings(rule, input, idx);
    for (unsigned int i = 0; i < rule.num_result_words; i++){
        LResultWord res = rule.result[i];
        LWord out = (LWord){.name = res.name, .num_values = res.num_calculations};
        for (unsigned int j = 0; j < res.num_calculations; j++){
            EvaluationResult ev_res = evaluate_ast(bindings, res.calculations[i]);
            if (ev_res.type != EVALUATION_RESULT_DBL) {
                out.values[i] = 0;
            } else {
                out.values[i] = ev_res.dbl_val;
            }
        }
        output->content[idx] = out;
        idx++;
    }
}

LString * apply_rules(LRuleset rules, LString * input){
    LString * output = malloc(sizeof(LString));
    output->capacity = input->capacity < 1;
    output->length = 0;
    output->content = malloc(sizeof(LWord) * output->capacity);
    memset(output->content, 0, output->capacity);
    bool found_rule = false;
    for(unsigned int word_idx = 0; word_idx < input->length; word_idx++){
        found_rule = false;
        for(unsigned int rule_idx = 0; rule_idx < rules.num_rules; rule_idx++){
            LRule rule = *(rules.rules + rule_idx); 
            if (does_rule_apply(rule, input, word_idx)){
                found_rule = true;
                unsigned int append_len = rule.num_result_words;
                unsigned int cap_left = output->capacity - output->length;
                while (append_len >= cap_left) {
                    LWord * temp = realloc(output->content, output->capacity << 1);
                    if (temp == NULL) return NULL;
                    output->content = temp;
                    output->capacity = output->capacity << 1;
                    cap_left = output->capacity - output->length;
                }

                append_result(output, rule, input, word_idx);
                break;
            }
        }
        if (!found_rule){
            // just copy the letter verbatim then
            output->content[output->length] = input->content[word_idx];
            output->length++;
        }
    }
    return output;
}


