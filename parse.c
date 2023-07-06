// parse.c - parsing and parse tree construction

#include "project04.h"

void parse_table_init(struct parse_table_st *parse_table) {
    parse_table->root = NULL;
}

// Allocate a parse node from the table of parse nodes
struct parse_node_st * parse_node_new() {
    return calloc(1, sizeof(struct parse_node_st));
}

void parse_error(char *err) {
    printf("parse_error: %s\n", err);
    exit(-1);
}

// These are names of operators for printing
char *parse_oper_strings[] = {"PLUS", "MINUS", "MULT", "DIV","RSHIFT","LSHIFT","NOT","AND","OR","XOR"};

// Print the dots which represent tree depth in the output
void parse_tree_print_indent(int level) {
    level *= 2;
    for (int i = 0; i < level; i++) {
        printf(".");
    }
}

// Traverse the tree recursively to print out the structs
void parse_tree_print_expr(struct parse_node_st *node, int level) {
    parse_tree_print_indent(level);
    printf("EXPR ");

    if (node->type == EX_INTVAL) {
        printf("INTVAL %d\n", node->intval.value);
    } else if (node->type == EX_OPER2) {
        printf("OPER2 %s\n", parse_oper_strings[node->oper2.oper]);
        parse_tree_print_expr(node->oper2.left, level + 1);
        parse_tree_print_expr(node->oper2.right, level + 1);
    } else if (node->type == EX_OPER1) { //Support for Oper1
        printf("OPER1 %s\n", parse_oper_strings[node->oper1.oper]);
        parse_tree_print_expr(node->oper1.operand,level + 1);
    }
}

void parse_tree_print(struct parse_node_st *np) {
    parse_tree_print_expr(np, 0);
}

// Parse the "program" part of the EBNF
// A program is composed of an expression followed by EOT
struct parse_node_st * parse_program(struct scan_table_st *scan_table) {
    struct parse_node_st *root;

    root = parse_expression(scan_table);

    if (!scan_table_accept(scan_table, TK_EOT)) {
        parse_error("Expecting EOT");        
    }

    return root;                                       
}

//Helper Function to remove the redundant code in parse_expression
void *parse_expression_general(struct scan_table_st *scan_table, struct parse_node_st **node1, struct parse_node_st **node2, enum parse_oper_enum oper){
	scan_table_accept(scan_table, TK_ANY);
        (*node2) = parse_node_new();
        (*node2)->type = EX_OPER2;
        (*node2)->oper2.oper = oper;
        (*node2)->oper2.left = (*node1);
        (*node2)->oper2.right = parse_operand(scan_table);
        (*node1) = (*node2);
}

// Build the tree for expressions
// Expressions are defined in the EBNF as an operator followed
// by zero or more operator operand pairs
struct parse_node_st * parse_expression(struct scan_table_st *scan_table) {
    struct scan_token_st *token;
    struct parse_node_st *node1, *node2;

    node1 = parse_operand(scan_table);

    while (true) {    
        token = scan_table_get(scan_table, 0);
        if (token->id == TK_PLUS) {
		parse_expression_general(scan_table, &node1, &node2, OP_PLUS);
        } else if (token->id == TK_MINUS) {
        	parse_expression_general(scan_table, &node1, &node2, OP_MINUS);    
        } else if (token->id == TK_DIV) {
		parse_expression_general(scan_table, &node1, &node2, OP_DIV);
        } else if (token->id == TK_MULT) {
		parse_expression_general(scan_table, &node1, &node2, OP_MULT);
        }  else if (token->id == TK_RSHIFT) {
		parse_expression_general(scan_table, &node1, &node2, OP_RSHIFT);
        }  else if (token->id == TK_LSHIFT) {
		parse_expression_general(scan_table, &node1, &node2, OP_LSHIFT);
        } else if (token->id == TK_NOT) {
		parse_expression_general(scan_table, &node1, &node2, OP_NOT);
        } else if (token->id == TK_AND) {
		parse_expression_general(scan_table, &node1, &node2, OP_AND);
        } else if (token->id == TK_OR) {
		parse_expression_general(scan_table, &node1, &node2, OP_OR);
        } else if (token->id == TK_XOR) {
		parse_expression_general(scan_table, &node1, &node2, OP_XOR);
        } else {
            break;
        }
    }

    return node1;
}

//Helper function for the parse_operand function for datatypes: integers, hexadecimals and binary numbers 
void parse_operand_datatype(struct scan_table_st *scan_table, struct parse_node_st **node,int base){
	struct scan_token_st *token;
	token = scan_table_get(scan_table, -1);
        (*node) = parse_node_new();
        (*node)->type = EX_INTVAL;
        (*node)->intval.value = string_to_int(token->name,base);
} 

//Helper function for parse_operand function for operands: -, ~ and ( )
void parse_operand_operands(struct scan_table_st *scan_table, struct parse_node_st **node, enum parse_oper_enum oper){
	struct scan_token_st *token;
        token = scan_table_get(scan_table, -1);
        (*node) = parse_node_new();
        (*node)->type = EX_OPER1;
        (*node)->oper1.oper = oper;
        (*node)->oper1.operand = parse_operand(scan_table);
}


// Parse operands, which are defined in the EBNF to be 
// integer literals or unary minus or expressions 
struct parse_node_st *parse_operand(struct scan_table_st *scan_table) {
    struct scan_token_st *token;
    struct parse_node_st *node;

    if (scan_table_accept(scan_table, TK_INTLIT)) {
	    parse_operand_datatype(scan_table,&node,10);
    } else if (scan_table_accept(scan_table, TK_HEXLIT)) {
	     parse_operand_datatype(scan_table,&node,16);
    } else if (scan_table_accept(scan_table, TK_BINLIT)) {
	    parse_operand_datatype(scan_table, &node,2);
    } else if (scan_table_accept(scan_table, TK_MINUS)) {
	    parse_operand_operands(scan_table, &node, OP_MINUS);
    } else if (scan_table_accept(scan_table, TK_NOT)) {
	    parse_operand_operands(scan_table, &node, OP_NOT);
    } else if (scan_table_accept(scan_table, TK_LPAREN)) {
        token = scan_table_get(scan_table, -1);
        node = parse_expression(scan_table);
	node->type = EX_OPER2;
	scan_table_accept(scan_table,TK_RPAREN);
    } else {
        parse_error("Bad operand");
    }
    return node;
}

