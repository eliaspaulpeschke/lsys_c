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
    bool valid;
    bool is_set[26];
    double value[26];
} BindingValueList;

int jump_brack_left(LString * input, unsigned int idx){
    if (input->content[idx].name != ']') return -1;
    unsigned int stack = 0;
    while(idx > 0){
        if (input->content[idx].name == ']'){
            stack += 1;
        } else if (input->content[idx].name == '['){
            stack -= 1;
        }
        if(stack == 0 && input->content[idx-1].name != ']') return idx-1;
        idx -= 1;
    }
    return -1;
}

int jump_brack_right(LString * input, unsigned int idx){
    if (input->content[idx].name != '[') return -1;
    unsigned int stack = 0;
    while(idx < input->length -1){
        if (input->content[idx].name == '['){
            stack += 1;
        } else if (input->content[idx].name == ']'){
            stack -= 1;
        }
        if(stack == 0 && input->content[idx+1].name != '[') return idx+1;
        idx += 1;
    }
    return -1;
}

bool check_inside_brack_right(LRule rule, LString * input, unsigned int idx, unsigned int ctx_idx){
    idx++;
    unsigned int len = rule.r_context_size;
    while(ctx_idx < len && idx < input->length){
        if (input->content[idx].name == '['){
            if(check_inside_brack_right(rule, input, idx, ctx_idx)) return true;
            idx = jump_brack_right(input, idx);
        }else if (input->content[idx].name == ']'){
            return false;
        }else {
            if (rule.r_context[ctx_idx].name != input->content[idx].name) return false;
            idx++;
            ctx_idx++;
        }
    }
    if (ctx_idx == len && idx <= input->length) {
        return true;
    }
    return false;
}

BindingValueList bindings_inside_brack_right(LRule rule, LString * input, BindingValueList b, unsigned int idx, unsigned int ctx_idx){
    b.valid = true;
    idx++;
    unsigned int len = rule.r_context_size;
    BindingValueList temp; 

    while(ctx_idx < len && idx < input->length){
        if (input->content[idx].name == '['){
            temp = bindings_inside_brack_right(rule, input, b, idx, ctx_idx);
            if(temp.valid) return temp;
            idx = jump_brack_right(input, idx);
        }else if (input->content[idx].name == ']'){
            return (BindingValueList){.valid = false};
        }else {
            if (rule.r_context[ctx_idx].name != input->content[idx].name) return (BindingValueList){.valid = false};
            for (int i = 0; i < rule.r_context[ctx_idx].num_bindings; i++){
                unsigned int bind_idx = char2idx(rule.r_context[ctx_idx].bindings[i]);
                if (input->content[idx].num_values > i){
                    if (b.is_set[bind_idx]){
                        TraceLog(LOG_WARNING, "Double Binding in right context. Overriding."); 
                    }
                    b.is_set[bind_idx] = true;
                    b.value[bind_idx] = input->content[idx].values[i];
                }
            }
            idx++;
            ctx_idx++;
        }
    }
    if (ctx_idx == len && idx <= input->length) {
        return b;
    }
        return (BindingValueList){.valid = false};
}


bool check_letterwise_apply(LRule rule, LString * input, unsigned int idx){
    LRuleWord p = rule.premise;
    LWord w = input->content[idx];
    if (p.name != w.name) return false;
    unsigned int word_idx;
    unsigned int jmp_idx;
    LRuleWord lc;
    LRuleWord rc;
    word_idx = idx;
    for (unsigned int i = 0; i < rule.l_context_size; i++){
        word_idx -= 1; 
        if (word_idx < 0) return false;
        jmp_idx = jump_brack_left(input, word_idx);
        if (jmp_idx != -1) word_idx = jmp_idx;
        lc = rule.l_context[i];
        w = input->content[word_idx]; 
        if (w.name != lc.name) return false;
    }
    return check_inside_brack_right(rule, input, idx, 0);
//    return true;
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
    BindingValueList b = {true, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
    LRuleWord p = rule.premise;
    LWord w = input->content[idx];
    b = evaluate_words_bindings(b, p, w);
    LRuleWord lc;
    LRuleWord rc;
    unsigned int word_idx = idx;
    unsigned int jmp_idx;
    if (rule.l_context_size > idx) return b;
    if (idx + rule.r_context_size >= input->length) return b;

    for (unsigned int i = 0; i < rule.l_context_size; i++){
        word_idx -=1;
        if (word_idx < 0) break;
        jmp_idx = jump_brack_left(input, word_idx);
        if (jmp_idx != -1) word_idx = jmp_idx;
        lc = rule.l_context[i];
        w = input->content[word_idx]; 
        b = evaluate_words_bindings(b, lc, w);
    }
    word_idx = idx;
    BindingValueList b2 = bindings_inside_brack_right(rule, input, b, idx, 0);
    if (b2.valid == false) return b; 
    return b2;
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
            return (EvaluationResult){.type = EVALUATION_RESULT_DBL
                                     ,.dbl_val = node.payload.dbl_val};
        case LPAYLOAD_string:
            return (EvaluationResult){ .type = EVALUATION_RESULT_STRING
                                     , .string_val = node.payload.string_val};
        case LPAYLOAD_letter: 
            b_idx = char2idx(node.payload.letter_val);
            if (bindings.is_set[b_idx]){
                return (EvaluationResult){ .type = EVALUATION_RESULT_DBL
                                   , .dbl_val = bindings.value[b_idx]};
            } else {
                return (EvaluationResult){EVALUATION_RESULT_EMPTY};
            }
        case LPAYLOAD_bool:
            return (EvaluationResult){.type = EVALUATION_RESULT_BOOL, .bool_val = node.payload.bool_val};
        case LPAYLOAD_unary_op_dbl:
            if (node.child_count < 1) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_DBL) return empty;
            dbl_res = node.payload.unary_op_dbl_val(tmp1.dbl_val);
            return (EvaluationResult){ .type = EVALUATION_RESULT_DBL, .dbl_val = dbl_res};
        case LPAYLOAD_binary_op_dbl:
            if (node.child_count < 2) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_DBL) return empty;
            tmp2 = evaluate_ast(bindings, *node.child[1]);
            if (tmp2.type != EVALUATION_RESULT_DBL) return empty;
            dbl_res = node.payload.binary_op_dbl_val(tmp1.dbl_val, tmp2.dbl_val);
            return (EvaluationResult){.type = EVALUATION_RESULT_DBL, .dbl_val = dbl_res};
        case LPAYLOAD_unary_op_bool:
            if (node.child_count < 1) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_BOOL) return empty;
            bool_res = node.payload.unary_op_bool_val(tmp1.bool_val);
            return (EvaluationResult){.type = EVALUATION_RESULT_BOOL, .bool_val = bool_res};
        case LPAYLOAD_binary_op_dbl_bool:
            if (node.child_count < 2) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_DBL) return empty;
            tmp2 = evaluate_ast(bindings, *node.child[1]);
            if (tmp2.type != EVALUATION_RESULT_DBL) return empty;
            bool_res = node.payload.binary_op_dbl_bool_val(tmp1.dbl_val, tmp2.dbl_val);
            return (EvaluationResult){.type=EVALUATION_RESULT_BOOL,.bool_val = bool_res};
        case LPAYLOAD_binary_op_bool_bool:
            if (node.child_count < 2) return empty;
            tmp1 = evaluate_ast(bindings, *node.child[0]);
            if (tmp1.type != EVALUATION_RESULT_BOOL) return empty;
            tmp2 = evaluate_ast(bindings, *node.child[1]);
            if (tmp2.type != EVALUATION_RESULT_BOOL) return empty;
            bool_res = node.payload.binary_op_bool_bool_val(tmp1.bool_val, tmp2.bool_val);
            return (EvaluationResult){.type=EVALUATION_RESULT_BOOL, .bool_val= bool_res};
    }
}

bool does_rule_apply(LRule rule, LString * input, unsigned int idx){
    if (rule.l_context_size > idx) return false;
    if (idx + rule.r_context_size >= input->length) return false;

    LWord word = input->content[idx];
    if (word.name != rule.premise.name) return false;

    if (!check_letterwise_apply(rule, input, idx)) return false;
    if (rule.qualifier == NULL) return true;
    BindingValueList bindings = evaluate_bindings(rule, input, idx);
    EvaluationResult res = evaluate_ast(bindings, *rule.qualifier);
    if (res.type != EVALUATION_RESULT_BOOL) return false;
    return res.bool_val;
}

void append_result(LString * output, LRule rule, LString * input, unsigned int idx){
    BindingValueList bindings = evaluate_bindings(rule, input, idx);
    for (unsigned int i = 0; i < rule.num_result_words; i++){
        LResultWord res = rule.result[i];
        LWord out = (LWord){.name = res.name, .num_values = res.num_calculations, .values = NULL};
        if (res.num_calculations >0){
            double * outvals = calloc(res.num_calculations, sizeof(double));
            out.values = outvals;
            for (unsigned int j = 0; j < res.num_calculations; j++){
                EvaluationResult ev_res = evaluate_ast(bindings, res.calculations[j]);
                if (ev_res.type != EVALUATION_RESULT_DBL) {
                    out.values[j] = 0;
                } else {
                    out.values[j] = ev_res.dbl_val;
                }
            }
        }
        output->content[output->length] = out;
        output->length++;
    }
}

LString * apply_rules(LRuleset rules, LString * input){
    TraceLog(LOG_DEBUG, "Got lstring %s", lstring_to_string(input));

    // TODO: this is a memory leak machine
    LString * output = malloc(sizeof(LString));
    output->capacity = input->length << 2;
    output->length = 0;
    output->content = calloc(output->capacity, sizeof(LWord));
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
                    LWord * temp = realloc(output->content, output->capacity << 1 * sizeof(LWord));
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
    TraceLog(LOG_DEBUG, "Return lstring %s", lstring_to_string(output));
    return output;
}

LString * apply_rules_n(LRuleset rules, LString * input, unsigned int n){
    // TODO: this is a memory leak machine
    LString * out;
/*    LString * tmp = malloc(sizeof(LString));
    memcpy(tmp, input, sizeof(LString));
    tmp->content = malloc(sizeof(LWord) * tmp->capacity);
    memcpy(tmp->content, input->content, sizeof(LWord) * tmp->capacity);
*/
    if (n == 0) {
      LString * tmp = malloc(sizeof(LString));
      memcpy(tmp, input, sizeof(LString));
      tmp->content = calloc(tmp->capacity, sizeof(LWord));
      memcpy(tmp->content, input->content, sizeof(LWord) * tmp->capacity);
      return tmp;
    };
    out = apply_rules(rules, input);
    for (int i = 0; i < n - 1; i++){
        LString * tmp = apply_rules(rules, out);
        free(out->content);
        free(out);
        out = tmp;
    }
    return out;
}

typedef enum {
     PARSE_STATE_INIT
   , PARSE_STATE_VALUES
}LSTRING_PARSE_STATE;

void realloc_if_needed(LString * str){
    if (str->length >= str->capacity -1){
        unsigned int new = str->capacity << 1;
        LWord * temp = realloc(str->content, new);
        if (!temp) return;
        str->content = temp;
        memset(str->content + str->length, 0, new - str->length);
        str->capacity = new;
    }
}

void add_value(LWord * word, double val){
    if (word->num_values == 0){
        word->values = malloc(sizeof(double));
        word->values[0] = val;
        word->num_values = 1;
    }else{
        double * temp = realloc(word->values, sizeof(double) * (word->num_values + 1));
        if(temp == NULL) return;
        temp[word->num_values] = val;
        word->num_values++;
        word->values = temp;
    }
}

LString parse_lstring(char*input){
    char * pos = input;
    char buffer[64]; 
    memset(buffer, 0, 64);
    unsigned int bufferpos = 0;
    LSTRING_PARSE_STATE state = PARSE_STATE_INIT;
    LString out = (LString){.capacity = 64, .length = 0, .content = malloc(sizeof(LWord)*64)};
    while(*pos != '\0'){
        switch (state) {
            case PARSE_STATE_INIT:
                if(*pos == '('){
                    state = PARSE_STATE_VALUES;
                }else{
                    realloc_if_needed(&out);
                    out.content[out.length] = (LWord){.name = *pos
                                , .num_values = 0, .values = NULL};
                    out.length++;
                }
                break;
            case PARSE_STATE_VALUES:
                if(*pos == ')'){
                    add_value(&out.content[out.length-1], strtod(buffer,NULL));
                    memset(buffer, 0, 64);
                    state = PARSE_STATE_INIT;
                    bufferpos = 0;
                } else if(*pos == ','){
                    add_value(&out.content[out.length-1], strtod(buffer,NULL));
                    memset(buffer, 0, 64);
                    bufferpos = 0;
                }else{
                    if (bufferpos < 64){
                        buffer[bufferpos] = *pos;
                        bufferpos+=1;
                    }
                }
                break;
            default:
                break;
        }
        pos++;
    }
    return out;
}

void sprintf_step(int * pos, int * len, char ** tgt, const char * format, ...){
    va_list argptr;
    bool through = false;
    while (!through){

        va_start(argptr, format);
        int res = vsnprintf( (*tgt) + (*pos)
                           , (*len) - (*pos)
                           , format
                           , argptr );
        va_end(argptr);

        if (res > (*len) - (*pos)) {
            char * temp = realloc(*tgt, (*len) << 1);
            if (temp == NULL) return;
            *tgt = temp;
            *len <<= 1;
        } else {
            through = true;
            *pos += res;
        }
    }
}

char * lstring_to_string(LString * s){
    int len = s->length << 2;
    char * out = calloc(len, sizeof(char)); 
    int pos = 0;

    for (int i = 0; i < s->length; i++){
        sprintf_step(&pos, &len, &out, "%c", s->content[i].name);
        if (s->content[i].num_values <= 0) continue;
        sprintf_step(&pos, &len, &out, "(");
        for (int j = 0; j < s->content[i].num_values; j++){
            sprintf_step(&pos, &len, &out, "%f", s->content[i].values[j]);
            if(j < s->content[i].num_values -1){
                sprintf_step(&pos, &len, &out, " ,");
            }
        }
        sprintf_step(&pos, &len, &out, ")");
    }

    return out;
}
