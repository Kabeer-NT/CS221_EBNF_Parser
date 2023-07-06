#include "project04.h"

int main(int argc, char **argv) {

    struct scan_table_st scan_table; // table of tokens
    struct parse_table_st parse_table; // table of parse nodes
    struct parse_node_st *parse_tree; // tree (pointers only) of parse nodes

    char input[SCAN_INPUT_LEN];
    int len;
    int base = 10; //Default Base is set to 10
    int width = 32;

    for(int i = 1; i < argc; i++){
    	if(!strncmp(argv[i],"-b",2)){
		base = atoi(argv[i+1]);
		i++;
	} else if(!strncmp(argv[i],"-e",2)){
		strncpy(input,argv[i+1],SCAN_INPUT_LEN);
		len = strnlen(input, SCAN_INPUT_LEN);
		i++;
	} else if(!strncmp(argv[i],"-w",2)){
                width = atoi(argv[i+1]);
		i++;
    	}else {
		//Error checking to see if not enough arguments are given or if invalid arguments are given
		printf("Invalid Argument Type\n");
		printf("Usage: ./project04 -e <expression> -b <base> [-b is optional]\n");
		printf("Example: ./project04 -e \"10+1\" -b 16");
	}
    }

    scan_table_init(&scan_table);
    scan_table_scan(&scan_table, input, len);

    parse_table_init(&parse_table);
    parse_tree = parse_program(&scan_table);

    //Calculate and convert the answer with the required base
    convert(eval_tree(parse_tree), base, width);
        
    return 0;
}
