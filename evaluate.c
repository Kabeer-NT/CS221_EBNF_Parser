#include "project04.h"

//Function to convert the given decimal to the required form [Extra Credit: in the required size]
void convert(unsigned int val, int base, int width){
    	if(base == 10){ 
        	printf("%d", val); //If base is 10 then just print the val -> No problems
		return ;
    	} else if(base == 16){
        	width /= 4; //Since each hex char is 4 bits we need the size/ 4 bits for it
        	printf("0x"); //Print the hex signature
    	} else if(base == 2){
        	printf("0b"); //Print the binary signature
    	}
    
    	char output[width]; //Initialised output string 
    	int iter = 0; //Initialising interator variable
   
       	//This part of the code converts the decimal to hex/binary in the required size and fill its with 0s if not all bits are used.
	//NOTE: This part of the code makes the string in reverse and hence when we print down below we print in reverse
   	while(iter != width){
        	if(val == 0){
            		output[iter] = 48;
        	} else if(val % base > 9){
            		output[iter] = val % base + 55;
        	} else {
            		output[iter] = val % base + 48;
        	}
        	iter++;
        	val /= base;
    	}
     	
	//Print it in reverse (Refer to the comment above)
    	for(int i = iter - 1; i >= 0 ; i--){
        	printf("%c",output[i]);
    	}
}

//Function to evalute the tree using Breadth First Search
int eval_tree(struct parse_node_st *node){
	if(node->type == EX_INTVAL){
		return node->intval.value;
	} else if(node->type == EX_OPER2){
		int left = eval_tree(node->oper2.left);
		int right = eval_tree(node->oper2.right);
		//All Oper2 Operands
		if(node->oper2.oper == OP_PLUS){
			return left + right;
		} else if(node->oper2.oper == OP_MINUS){
                        return left - right;
                } else if(node->oper2.oper == OP_MULT){
                        return left * right;
                } else if(node->oper2.oper == OP_DIV){
                        return left / right;
                } else if(node->oper2.oper == OP_RSHIFT){
                        return left >> right;
                } else if(node->oper2.oper == OP_LSHIFT){
                        return left << right;
                }  else if(node->oper2.oper == OP_AND){
                        return left & right;
                } else if(node->oper2.oper == OP_OR){
                        return left | right;
                }  else if(node->oper2.oper == OP_XOR){
                        return left ^ right;
                }  else {
          		printf("Invalid Operand");
			exit(-1);
                }
	} else if(node->type == EX_OPER1){
		int val = eval_tree(node->oper1.operand);
		if(node->oper1.oper == OP_MINUS){
			return -1 * val;
		} else if(node->oper1.oper == OP_NOT){
			return ~val;
		} else {
                        printf("Invalid Unary Operand");
                        exit(-1);
                }
	}
}
