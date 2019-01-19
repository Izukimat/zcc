#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Values that describe the type of Token
enum {
	TK_NUM = 256, // Integer Token
	TK_EOF,		  // End Token

};

// Type of Token
typedef struct {
	int ty;		// type of Token
	int val;	// Value of ty if ty is TK_NUM
	char *input;// Token string for error message
} Token;

//	an array to store the result of tokenization up to 100.
Token tokens[100];

// split the string by p into token
// save it to tokens
void tokenize(char *p){
	int i = 0;
	while (*p) {
		// skip empty letters
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p == '+' || *p == '-') {
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}
		
		if (isdigit(*p)) {
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}

		fprintf(stderr, "Tokenize Error: %s\n", p);
		exit(1);
	}
	
	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}

// Error reporting function
void error(int i) {
	fprintf(stderr, "Unexpected Token: %s\n",
			tokens[i].input);
	exit(1);
}

int main(int argc, char **argv){
    if (argc != 2){
      	fprintf(stderr, "Number of input is not correct\n");
    return 1;
    }
	
	
	tokenize(argv[1]);
	
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
   
	if (tokens[0].ty != TK_NUM)
		error(0);
	printf("	mov rax, %d\n", tokens[0].val);


	// Consume Token of '+ <num>' or '- <num>'
	// output assembly

	int i = 1;
	while (tokens[i].ty != TK_EOF){
		if (tokens[i].ty == '+'){
			i++;
			if (tokens[i].ty != TK_NUM)
				error(i);
			printf("  add rax, %d\n", tokens[i].val);
			i++;
			continue;
		}

		if (tokens[i].ty == '-'){
			i++;
			if (tokens[i].ty != TK_NUM)
				error(i);
			printf("  sub rax, %d\n", tokens[i].val);
			i++;
			continue;			
		}

		error(i);
	}

	printf("  ret\n");
	return 0;

}
