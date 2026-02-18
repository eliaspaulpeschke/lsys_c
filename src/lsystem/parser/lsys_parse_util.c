#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lsys_parse_util.h"

// TODO: think about how we free this!
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
        , .error = (ParserErr){.error_text = NULL, .has_errored = false}
    };
    return pst;
}

ParseData * mk_parse_data(ParseDataType type){
    ParseData * pd = malloc(sizeof(ParseData));
    *pd = (ParseData) {.type = type, 0};
    return pd;
};

ParseData * mk_node_empty(){
    ParseData * pd = mk_parse_data(PARSE_DATA_empty);
    return pd;
}

ParseData * mk_node_pd(LPayload payload){
    ParseData * pd = mk_parse_data(PARSE_DATA_LAstNode);
    if (pd == NULL) return NULL;
    LAstNode * ln = mk_node(payload);
    *pd = (ParseData) {
          .type = PARSE_DATA_LAstNode
        , .list_length = 1
        , .lastnode_val = ln
    };
    return pd;
}

bool is_parse_data_empty(ParseData *pd){
    if (pd == NULL) return true;
    if (pd->type == PARSE_DATA_empty) return true;  
    if (pd->list_length == 0) return true;
    if (pd->parse_data_val == NULL) return true;
    return false;
}

bool pd_is_ast(ParseData * pd){
    return (pd->type == PARSE_DATA_LAstNode);
}

bool pd_is_dbl(ParseData * pd){
    if (!pd_is_ast(pd)) return false;
    if (!(pd->lastnode_val->payload.type == LPAYLOAD_dbl
       ||pd->lastnode_val->payload.type == LPAYLOAD_letter //a binding
       ||pd->lastnode_val->payload.type == LPAYLOAD_unary_op_dbl 
       ||pd->lastnode_val->payload.type == LPAYLOAD_binary_op_dbl))
       return false;
    return true;
}

bool pd_is_dbl_lit(ParseData * pd){
    if (!pd_is_ast(pd)) return false;
    return (pd->lastnode_val->payload.type == LPAYLOAD_dbl);
}

double dbl_lit_value(ParseData * pd){
    if(!pd_is_dbl_lit(pd)) return 0;
    return (pd->lastnode_val->payload.dbl_val);
}

bool pd_is_bool_lit(ParseData * pd){
    if (!pd_is_ast(pd)) return false;
    return (pd->lastnode_val->payload.type == LPAYLOAD_bool);
}

bool bool_lit_value(ParseData * pd){
    if(!pd_is_dbl_lit(pd)) return 0;
    return (pd->lastnode_val->payload.bool_val);
}

bool pd_is_bool(ParseData * pd){
    if (!pd_is_ast(pd)) return false;
    if (!(pd->lastnode_val->payload.type == LPAYLOAD_bool
       ||pd->lastnode_val->payload.type == LPAYLOAD_unary_op_bool
       ||pd->lastnode_val->payload.type == LPAYLOAD_binary_op_bool_bool
       ||pd->lastnode_val->payload.type == LPAYLOAD_binary_op_dbl_bool))
       return false;
    return true;
}

ParseData * mk_node_dbl_lit(double x){
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_dbl, .dbl_val = x});
    return pd;
}

ParseData * mk_node_bool_lit(bool x){
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_bool, .bool_val = x});
    return pd;
}

ParseData * mk_node_letter_lit(char x){
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_letter, .letter_val = x});
    return pd;
}

ParseData * mk_node_string_lit(char * x){
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_string, .string_val = x});
    return pd;
}

ParseData * mk_node_bdbl(ParseData * l, ParseData * r, bool free_child_data, binary_op_dbl fn){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd; 
    if (pd_is_dbl_lit(l) && pd_is_dbl_lit(r)) {
        pd = mk_node_dbl_lit(fn(dbl_lit_value(l), dbl_lit_value(r)));
    } else {
        pd = mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl, .binary_op_dbl_val = fn});
        node_add_child(pd->lastnode_val, l->lastnode_val);
        node_add_child(pd->lastnode_val, r->lastnode_val);
    }
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}

ParseData * mk_node_binminus(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_bdbl(l,r,free_child_data, lop_binary_minus);
}
ParseData * mk_node_binplus(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_bdbl(l,r,free_child_data, lop_binary_plus);
}
ParseData * mk_node_multiply(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_bdbl(l,r,free_child_data, lop_multiply);
}
ParseData * mk_node_divide(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_bdbl(l,r,free_child_data, lop_divide);
}

ParseData * mk_node_udbl(ParseData * x, bool free_child_data, unary_op_dbl fn){
    if (!pd_is_dbl(x)) return NULL;

    ParseData * pd;
    if (pd_is_dbl_lit(x)) {
        pd = mk_node_dbl_lit(fn(dbl_lit_value(x)));
    } else {
        pd = mk_node_pd((LPayload){.type = LPAYLOAD_unary_op_dbl, .unary_op_dbl_val = fn});
        node_add_child(pd->lastnode_val, x->lastnode_val);
    }
    if (free_child_data){
        free(x);
    }
    return pd;
}

ParseData * mk_node_uplus(ParseData * x, bool free_child_data){
    return mk_node_udbl(x,free_child_data, lop_unary_plus);
}
ParseData * mk_node_uminus(ParseData * x, bool free_child_data){
    return mk_node_udbl(x,free_child_data, lop_unary_minus);
}

ParseData * mk_node_dbl_bool(ParseData * l, ParseData * r, bool free_child_data, binary_op_dbl_bool fn){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd; 
    if (pd_is_dbl_lit(l) && pd_is_dbl_lit(r)) {
        pd = mk_node_bool_lit(fn(dbl_lit_value(l), dbl_lit_value(r)));
    } else {
        pd =  mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl_bool, .binary_op_dbl_bool_val = fn});
        node_add_child(pd->lastnode_val, l->lastnode_val);
        node_add_child(pd->lastnode_val, r->lastnode_val);
    }
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}

ParseData * mk_node_eq(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_dbl_bool(l, r, free_child_data, lop_eq);
}
ParseData * mk_node_lt(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_dbl_bool(l, r, free_child_data, lop_lt);
}
ParseData * mk_node_gt(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_dbl_bool(l, r, free_child_data, lop_gt);
}
ParseData * mk_node_leq(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_dbl_bool(l, r, free_child_data, lop_leq);
}
ParseData * mk_node_geq(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_dbl_bool(l, r, free_child_data, lop_geq);
}
ParseData * mk_node_neq(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_dbl_bool(l, r, free_child_data, lop_neq);
}

ParseData * mk_node_ubool(ParseData * x, bool free_child_data, unary_op_bool fn){
    if (!pd_is_bool(x)) return NULL;
    ParseData * pd;
    if (pd_is_bool_lit(x)) {
        pd = mk_node_bool_lit(fn(bool_lit_value(x)));
    } else {
        pd =  mk_node_pd((LPayload){.type = LPAYLOAD_unary_op_bool, .unary_op_bool_val = fn});
        node_add_child(pd->lastnode_val, x->lastnode_val);
    }
    if (free_child_data){
        free(x);
    }
    return pd;
}

ParseData * mk_node_not(ParseData * x, bool free_child_data){
    return mk_node_ubool(x, free_child_data, lop_not);
}

ParseData * mk_node_bool_bool(ParseData * l, ParseData * r, bool free_child_data, binary_op_bool_bool fn){
    if (!pd_is_bool(r) || !pd_is_bool(l)) return NULL;
    ParseData * pd;
    if (pd_is_bool_lit(l) && pd_is_bool_lit(r)) {
        pd = mk_node_bool_lit(fn(bool_lit_value(l), bool_lit_value(r)));
    } else {
        pd =  mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_bool_bool, .binary_op_bool_bool_val = fn});
        node_add_child(pd->lastnode_val, l->lastnode_val);
        node_add_child(pd->lastnode_val, r->lastnode_val);
    }
    if (free_child_data){
        free(r);
        free(l);
    }
    return pd;
}

ParseData * mk_node_and(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_bool_bool(l, r, free_child_data, lop_and);
}
ParseData * mk_node_or(ParseData * l, ParseData * r, bool free_child_data){
    return mk_node_bool_bool(l, r, free_child_data, lop_or);
}

ParseData * pd_node_append(ParseData * x, ParseData * y, bool free_old_data){
    unsigned int len = x->list_length;
    if (x->type != y->type) return x;
    ParseData * pd;
    switch (x->type) {
        case PARSE_DATA_LAstNode:
           pd = mk_parse_data(PARSE_DATA_LAstNode);
           pd->list_length = len+1;
           pd->lastnode_val = malloc(sizeof(LAstNode) * (len+1));
           memcpy(pd->lastnode_val, x->lastnode_val, sizeof(LAstNode) * len);
           memcpy(pd->lastnode_val + len, y->lastnode_val , sizeof(LAstNode));
           break;
        case PARSE_DATA_LResultWord:
           pd = mk_parse_data(PARSE_DATA_LResultWord);
           pd->list_length = len+1;
           pd->lresultword_val = malloc(sizeof(LResultWord) * (len+1));
           memcpy(pd->lresultword_val, x->lresultword_val, sizeof(LResultWord) * (len));
           memcpy(pd->lresultword_val + len, y->lresultword_val, sizeof(LResultWord));
           break;
        case PARSE_DATA_LRuleWord:
           pd = mk_parse_data(PARSE_DATA_LRuleWord);
           pd->list_length = len+1;
           pd->lruleword_val = malloc(sizeof(LRuleWord) * (len+1));
           memcpy(pd->lruleword_val, x->lruleword_val, sizeof(LRuleWord) * (len));
           memcpy(pd->lruleword_val + len, y->lruleword_val, sizeof(LRuleWord));
           break;
        case PARSE_DATA_LRule:
           pd = mk_parse_data(PARSE_DATA_LRule);
           pd->list_length = len+1;
           pd->lrule_val = malloc(sizeof(LRule) * (len+1));
           memcpy(pd->lrule_val, x->lrule_val, sizeof(LRule) * (len));
           memcpy(pd->lrule_val + len, y->lrule_val, sizeof(LRule));
           break;
        case PARSE_DATA_ParseData:
           pd = mk_parse_data(PARSE_DATA_ParseData);
           pd->list_length = len+1;
           pd->parse_data_val= malloc(sizeof(ParseData) * (len+1));
           memcpy(pd->parse_data_val, x->parse_data_val, sizeof(ParseData) * (len));
           memcpy(pd->parse_data_val+ len, y->parse_data_val, sizeof(ParseData));
           break;
        default:
            return x;
    }
    if (free_old_data){
        free(x);
        free(y);
    }
    return pd;
}

ParseData * mk_pd_resultword(char letter, ParseData * ast, bool free_old){
    ParseData * pd = mk_parse_data(PARSE_DATA_LResultWord);
    LResultWord * lrw = malloc(sizeof(LResultWord));
    pd->list_length = 1;
    lrw->name = letter;
    if ((ast == NULL) || (ast->type != PARSE_DATA_LAstNode)){
        lrw->num_calculations = 0;
        lrw->calculations = NULL;
    } else {
        lrw->num_calculations = ast->list_length;
        lrw->calculations = ast->lastnode_val;
    }
    if (free_old){
        free(ast);
    }
    pd->lresultword_val = lrw;
    return pd;
}

ParseData * mk_pd_ruleword(char letter, ParseData * ast, bool free_old){
    ParseData * pd = mk_parse_data(PARSE_DATA_LRuleWord);
    LRuleWord * lrw = malloc(sizeof(LRuleWord));
    pd->list_length = 1;
    lrw->name = letter;
     if ((ast == NULL) || (ast->type != PARSE_DATA_LAstNode)){
        lrw->num_bindings = 0;
        lrw->bindings = NULL;
    } else {
        lrw->bindings = malloc(ast->list_length + 1);
        for (int i = 0; i < ast->list_length; i++){
            LPayload pl = (ast->lastnode_val + i)->payload;
            lrw->bindings[i] = pl.letter_val;
        }
        lrw->bindings[ast->list_length] = '\0';
        lrw->num_bindings = ast->list_length;
    }
    if (free_old){
        free(ast);
    }
    pd->lruleword_val = lrw;
    return pd;
}

ParseData * mk_pd_rule(ParseData * name, ParseData * lc, ParseData * prem, ParseData * rc, ParseData * qualif, ParseData * result, bool free_old){
    ParseData * pd = mk_parse_data(PARSE_DATA_LRule);
    pd->list_length = 1;
    pd->lrule_val = malloc(sizeof(LRule));
    LRule * r = pd->lrule_val;
    r->name = NULL;
    if (!is_parse_data_empty(name)) r->name = name->string_val;
    r->l_context_size = 0;
    r->l_context = NULL;
    if (!is_parse_data_empty(lc)){
        r->l_context_size = lc->list_length;
        r->l_context = lc->lruleword_val;
    }
    r->r_context_size = 0;
    r->r_context = NULL;
    if (!is_parse_data_empty(rc)){
        r->r_context_size = rc->list_length;
        r->r_context = rc->lruleword_val;
    }
    if (is_parse_data_empty(prem)) {
        printf("ERROR, have no premise");
        return NULL;
    }
    r->premise = *prem->lruleword_val;
    r->qualifier = NULL;
    if (!is_parse_data_empty(qualif)){
        r->qualifier = qualif->lastnode_val;
    }
    if (is_parse_data_empty(result)) {
        printf("ERROR, have no result");
        return NULL;
    }
    r->result = result->lresultword_val;
    r->num_result_words = result->list_length;

    if(free_old){
        free(result);
        free(prem);
        if (rc != NULL) free(rc);
        if (lc != NULL) free(lc);
        if (qualif != NULL) free(qualif);
        if (name != NULL) free(name);
    }
    return pd;
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
