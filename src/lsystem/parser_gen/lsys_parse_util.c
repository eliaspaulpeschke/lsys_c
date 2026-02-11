#include "lsystem2.h"
#include "lsys_parse_util.h"

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

static inline bool pd_is_ast(ParseData * pd){
    return (pd->type == PARSE_DATA_LAstNode);
}

static inline bool pd_is_dbl(ParseData * pd){
    if (!pd_is_ast(pd)) return false;
    if (!(pd->lastnode_val->payload.type == LPAYLOAD_dbl
       ||pd->lastnode_val->payload.type == LPAYLOAD_letter //a binding
       ||pd->lastnode_val->payload.type == LPAYLOAD_unary_op_dbl 
       ||pd->lastnode_val->payload.type == LPAYLOAD_binary_op_dbl))
       return false;
    return true;
}

static inline bool pd_is_bool(ParseData * pd){
    if (!pd_is_ast(pd)) return false;
    if (!(pd->lastnode_val->payload.type == LPAYLOAD_bool
       ||pd->lastnode_val->payload.type == LPAYLOAD_unary_op_bool
       ||pd->lastnode_val->payload.type == LPAYLOAD_binary_op_bool_bool
       ||pd->lastnode_val->payload.type == LPAYLOAD_binary_op_dbl_bool))
       return false;
    return true;
}

ParseData * mk_node_binplus(ParseData * l, ParseData * r, bool free_child_data){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl, .binary_op_dbl_val = lop_binary_plus});
    node_add_child(pd->lastnode_val, l->lastnode_val);
    node_add_child(pd->lastnode_val, r->lastnode_val);
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}

ParseData * mk_node_binminus(ParseData * l, ParseData * r, bool free_child_data){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl, .binary_op_dbl_val = lop_binary_minus});
    node_add_child(pd->lastnode_val, l->lastnode_val);
    node_add_child(pd->lastnode_val, r->lastnode_val);
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}

ParseData * mk_node_multiply(ParseData * l, ParseData * r, bool free_child_data){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl, .binary_op_dbl_val = lop_multiply});
    node_add_child(pd->lastnode_val, l->lastnode_val);
    node_add_child(pd->lastnode_val, r->lastnode_val);
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}

ParseData * mk_node_divide(ParseData * l, ParseData * r, bool free_child_data){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl, .binary_op_dbl_val = lop_divide});
    node_add_child(pd->lastnode_val, l->lastnode_val);
    node_add_child(pd->lastnode_val, r->lastnode_val);
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}


ParseData * mk_node_uplus(ParseData * x, bool free_child_data){
    if (!pd_is_dbl(x)) return NULL;
    if (x->lastnode_val->payload.type == LPAYLOAD_dbl){
        x->lastnode_val->payload.dbl_val = +(x->lastnode_val->payload.dbl_val);
        return x;
    }
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_unary_op_dbl, .binary_op_dbl_val = lop_binary_plus});
    node_add_child(pd->lastnode_val, x->lastnode_val);
    if (free_child_data){
        free(x);
    }
    return pd;
}

ParseData * mk_node_uminus(ParseData * x, bool free_child_data){
    if (!pd_is_dbl(x)) return NULL;
    if (x->lastnode_val->payload.type == LPAYLOAD_dbl){
        x->lastnode_val->payload.dbl_val = -(x->lastnode_val->payload.dbl_val);
        return x;
    }
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_unary_op_dbl, .binary_op_dbl_val = lop_binary_minus});
    node_add_child(pd->lastnode_val, x->lastnode_val);
    if (free_child_data){
        free(x);
    }
    return pd;
}

ParseData * mk_node_dbl_bool(ParseData * l, ParseData * r, bool free_child_data){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl_bool, .binary_op_dbl_bool_val = lop_eq});
    node_add_child(pd->lastnode_val, l->lastnode_val);
    node_add_child(pd->lastnode_val, r->lastnode_val);
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}

ParseData * mk_node_lt(ParseData * l, ParseData * r, bool free_child_data){
    if (!pd_is_dbl(l) && pd_is_dbl(r)) return NULL;
    ParseData * pd = mk_node_pd((LPayload){.type = LPAYLOAD_binary_op_dbl_bool, .binary_op_dbl_bool_val = lop_lt});
    node_add_child(pd->lastnode_val, l->lastnode_val);
    node_add_child(pd->lastnode_val, r->lastnode_val);
    if (free_child_data){
        free(l);
        free(r);
    }
    return pd;
}


