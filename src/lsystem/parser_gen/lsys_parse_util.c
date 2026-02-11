#include "lsystem2.h"
#include <stdlib.h>
#include <string.h>
#include "lsys_parse_util.h"

// TODO: think about how we free this!

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

ParseData * mk_pd_resultword(char letter, ParseData * ast){
    ParseData * pd = mk_parse_data(PARSE_DATA_LResultWord);
    LResultWord * lrw = malloc(sizeof(LResultWord));
    lrw->name = letter;
    if ((ast == NULL) || (ast->type != PARSE_DATA_LAstNode)){
        lrw->num_calculations = 0;
        lrw->calculations = NULL;
    } else {
        lrw->num_calculations = ast->list_length;
        lrw->calculations = ast->lastnode_val;
    }
    pd->lresultword_val = lrw;
    return pd;
}
