#ifndef LSYS_PARSE_UTIL_H
#define LSYS_PARSE_UTIL_H
#include "lsystem2.h"

ParseData * mk_node_pd(LPayload payload);

bool pd_is_ast(ParseData * pd);
bool pd_is_dbl(ParseData * pd);
bool pd_is_bool(ParseData * pd);

ParseData * mk_node_dbl_lit(double x);
ParseData * mk_node_bool_lit(bool x);
ParseData * mk_node_letter_lit(char x);
ParseData * mk_node_string_lit(char * x);

ParseData * mk_node_bdbl(ParseData * l, ParseData * r, bool free_child_data, binary_op_dbl fn);

ParseData * mk_node_binminus(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_binplus(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_multiply(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_divide(ParseData * l, ParseData * r, bool free_child_data);

ParseData * mk_node_udbl(ParseData * x, bool free_child_data, unary_op_dbl fn);

ParseData * mk_node_uplus(ParseData * x, bool free_child_data);
ParseData * mk_node_uminus(ParseData * x, bool free_child_data);

ParseData * mk_node_dbl_bool(ParseData * l, ParseData * r, bool free_child_data, binary_op_dbl_bool fn);
ParseData * mk_node_eq(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_lt(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_gt(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_leq(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_geq(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_neq(ParseData * l, ParseData * r, bool free_child_data);

ParseData * mk_node_ubool(ParseData * x, bool free_child_data, unary_op_bool fn);
ParseData * mk_node_not(ParseData * x, bool free_child_data);

ParseData * mk_node_bool_bool(ParseData * l, ParseData * r, bool free_child_data, binary_op_bool_bool fn);
ParseData * mk_node_and(ParseData * l, ParseData * r, bool free_child_data);
ParseData * mk_node_or(ParseData * l, ParseData * r, bool free_child_data);

ParseData * mk_node_empty();

ParseData * pd_node_append(ParseData * x, ParseData * y, bool free_old_data);
ParseData * mk_pd_resultword(char letter, ParseData * ast);
#endif
