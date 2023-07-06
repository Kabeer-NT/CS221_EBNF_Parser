#include "project04.h"

int string_to_int(char *string, int base) {
    int sum = 0;
    int pv = 1;
    for(int i = strlen(string) - 1; i >= 0; i--){
	    if(string[i] >= '0' && string[i] <= '9'){
		    sum += (string[i]- '0') * pv;
            	    pv *= base;
	    } else if(string[i] >= 'a' && string[i] <= 'f'){
    		    sum += (string[i]- 87) * pv;
		    pv *= base;
	    } else if(string[i] >= 'A' && string[i] <= 'F'){
		    sum += (string[i]- 55) * pv;
		    pv *= base;
        }
    }
    return sum;
}
