/*
 * Implement for DFA for Language (abcd*)

 */

/* DFA :-(Q,sigma, transition function, start state, final state)
 * Q :- 0, 1, 2, 3, 4
 * sigma :- a, b ,c , d, ..., z
 * transition function :- (s,a) 	-> 0, 
			  (s,sigma-a) 	-> 4, 
			  (0,b)		-> 1, 
			  (0, sigma-b) 	-> 4,
			  (1,c) 	-> 2, 
			  (1, sigma-c) 	-> 4,  
			  (2,d) 	-> 3,
			  (2, sigma-d)	-> 4,
			  (3, sigma)	-> 3
 * start		:- 0
 * final(accept) 	:- 3
 * reject		:- 0,1,2,4
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int state = -1;
void DFA(char *str) {
	int len = strlen(str);
	int i =0;
	for (i = 0; i < len; i++) {
			if ( i == 0) {
				if (str[i] == 'a')
					state = 0;
				else
					state = 4;
			}
			else if ( i == 1) {
				if ((str[i] == 'b') && (state == 0))
					state = 1;
				else
					state = 4;
			}
			else if ( i == 2) {
				if ((str[i] == 'c') && (state == 1))
					state = 2;
				else 
					state = 4;
			}
			else if ( i == 3) {
				if((str[i] == 'd') && (state == 2))
					state = 3;
				else 
					state = 4;
			}
			else {
				if(state == 3) 
					state = 3;
				else
					state = 4;
			}
		}
	if (state == 3) 
		printf("Accept.\n");
	else
		printf("Reject.\n");
}
		

int main() {
	char str[100];
	printf ("Enter the string : ");
	scanf("%s", str);
	DFA(str);
	return 0;
}

