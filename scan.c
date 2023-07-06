#include "project04.h"

void scan_error(char *err) {
    printf("scan_error: %s\n", err);
    exit(-1);
}

void scan_table_init(struct scan_table_st *st) {
    st->head = NULL;
    st->len = 0;
    st->next = 0;
}

struct scan_token_st *scan_table_new_token(struct scan_table_st *tt) {
    struct scan_token_st *tp = calloc(1, sizeof(struct scan_token_st));
    if (!tt->head) {
        tt->head = tp;
    } else {
        struct scan_token_st *walk = tt->head;
        while (1) {
            if (!walk->next) {
                walk->next = tp;
                break;
            } else {
                walk = walk->next;
            }
        }
    }

    // increment the length of reserved tokens 
    tt->len += 1;
    return tp;
}

char *scan_read_token(struct scan_token_st *tp, char *p, int len,
                       enum scan_token_enum id) {
    /* Read a token starting a p for len characters.
       Update the given token with the token string and token id. */
    int i;

    tp->id = id;
    for (i = 0; i < len; i++) {
        tp->name[i] = *p;
        p += 1;
    }
    tp->name[i] = '\0';
    return p;
}

bool scan_is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool scan_is_whitespace(char c) {
    return (c == ' ' || c == '\t');
}

char * scan_whitespace(char *p, char *end) {
    while (scan_is_whitespace(*p) && (p < end)) {
        p += 1;
    }
    return p;
}

//Function to check if a char is there for hexlit i.e A-F and a-f
bool scan_is_hexlit_char(char c){
	return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
    
char *scan_intlit(struct scan_token_st *tp, char *p, char *end) {
	char *tmp = p;
	int intlen = 0;
	while(scan_is_digit(*tmp)){ //Read all digits until the first no digit is found and then scan it using the already given function (scan_read_token)
		tmp++;
		intlen++;
	}
	p = scan_read_token(tp, p, intlen, TK_INTLIT);
    	return p;
}

//Function to scan for hexlit given the signature (0x) has been found
char *scan_hexlit(struct scan_token_st *tp, char *p, char *end){
                int lenhex = 0;
                p+=2; //increment pointer to get rid of the 0x
                char* tmp = p; //initialise tmp so we dont modify the orignal

                while((scan_is_digit(*p) || scan_is_hexlit_char(*p))){
                        lenhex++;
                        p++;
                }
                p = scan_read_token(tp,tmp,lenhex,TK_HEXLIT);

}

//Function to scan for binlit given the signature (0b) has been found 
char *scan_binlit(struct scan_token_st *tp, char *p, char *end){
                int lenbin = 0;
                p+=2;
                char* tmp = p;

                while((scan_is_digit(*p) || scan_is_hexlit_char(*p))){
                        lenbin++;
                        p++;
                }
                p = scan_read_token(tp,tmp,lenbin,TK_BINLIT);

}

char *scan_token(struct scan_token_st *tp, char *p, char *end) {
	//Test cases for all the possible tokens
        if(*p == '0' && *(p+1) == 'x'){
        	p = scan_hexlit(tp,p,end);
        } else if(*p == '0' && *(p+1) == 'b'){
                p = scan_binlit(tp,p,end);
        } else if (p == end) {
        	p = scan_read_token(tp, p, 0, TK_EOT);
    	} else if (scan_is_whitespace(*p)) {
        	p = scan_whitespace(p, end);
        	p = scan_token(tp, p, end);
    	} else if (scan_is_digit(*p)) {
        	p = scan_intlit(tp, p, end);
    	} else if (*p == '+') {
        	p = scan_read_token(tp, p, 1, TK_PLUS);
    	} else if (*p == '-') {
                p = scan_read_token(tp, p, 1, TK_MINUS);
        } else if (*p == '*') {
                p = scan_read_token(tp, p, 1, TK_MULT);
        } else if (*p == '/') {
		 p = scan_read_token(tp, p, 1, TK_DIV);
	} else if (!strncmp(p,">>",2)){
                 p = scan_read_token(tp, p, 2, TK_RSHIFT);
        } else if (!strncmp(p,"<<",2)){
                 p = scan_read_token(tp, p, 2, TK_LSHIFT);
        } else if (*p == '~'){
		 p = scan_read_token(tp, p, 1, TK_NOT);
        } else if (*p == '&'){
                 p = scan_read_token(tp, p, 1, TK_AND);
        } else if (*p == '|'){
                 p = scan_read_token(tp, p, 1, TK_OR);
        } else if (*p == '^'){
                 p = scan_read_token(tp, p, 1, TK_XOR);
        } else if (*p == '(') {
                p = scan_read_token(tp, p, 1, TK_LPAREN);
        } else if (*p == ')') {
                p = scan_read_token(tp, p, 1, TK_RPAREN);
        } else{
		//Error checking for if a token is entered other than the acceptable one
		scan_error(p);
	}
	
    	return p;
}

void scan_table_scan(struct scan_table_st *st, char *input, int len) {
    struct scan_token_st *tp;
    char *p = input;
    char *end = p + len;

    while(true) {
        tp = scan_table_new_token(st);
        p = scan_token(tp,p,end);
        if (tp->id == TK_EOT) {
            break;
        }
    }
}

void scan_table_print(struct scan_table_st *tt) {

    // define a variable containing the list of token ID names
    char *id_names[NUM_TOKENS] = SCAN_TOKEN_STRINGS;

    struct scan_token_st *tp = tt->head;
    while (tp) {
        // print the ID and name of that token
        printf("%s(\"%s\")\n", id_names[tp->id], tp->name);
        tp = tp->next;
    }
}

// Return the current token pointer + i spaces
struct scan_token_st * scan_table_get(struct scan_table_st *st, int i) {
    struct scan_token_st *pt = st->head;
    int which = st->next + i;

    for (int i = 0; i < which; i++) {
        pt = pt->next;
    }
    return pt;
}

// Return the next token pointer (and increment the cursor) if the
// next token matches tk_expected or the wildcard TK_ANY
bool scan_table_accept(struct scan_table_st *st, enum scan_token_enum tk_expected) {
    struct scan_token_st *tp;

    if (tk_expected == TK_ANY) {
        st->next += 1;
        return true;
    }

    tp = scan_table_get(st, 0);

    if (tp->id == tk_expected) {
        st->next += 1;
        return true;
    }

    return false;
}


