#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Types of tokens
typedef enum {
	TK_RESERVED, // sign
	TK_NUM,		 // integer token
	TK_EOF,		 // EOF token
} TokenKind;

typedef struct Token Token;

// Token type
struct Token {
	TokenKind kind;	// the type of Token
	Token *next;	// next Token input
	int val;		// the value if TokenKind is TK_NUM
	char *str;		// token strings
};

// Token on the scope
Token *token;

// Error reporting function
// It takes the same argument with printf
void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// When the next token is the expected sign, move it one step forward
// and return True. Otherwise returns False.
bool consume(char op){
	if (token->kind != TK_RESERVED || token ->str[0] != op)
		return false;
	token = token->next;
	return true;
}

// When the next token is the expected sign, move it one step forward.
// Otherwise, raise an error.
void expect(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op)
		error("It is not '%c'", op);
	token = token->next;
}

// When the next token is a numerical value, move it one step forward.
// Otherwise raise an error.
int expect_number() {
	if (token->kind != TK_NUM)
		error("It is not a number");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

// Create a new token and connect it to cur.
Token *new_token(TokenKind kind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// Tokenize the input string p and return it.
Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p){
		// Skip empty spaces.
		if (isspace(*p)){
			p++;
			continue;
		}

		if (*p == '+' || *p == '-'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("Cannot tokenize!");
	}

	new_token(TK_EOF, cur, p);
	return head.next;

}

int main(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "Wrong number of input.");
		return 1;
	}

	// Tokenize
	token = tokenize(argv[1]);

	// Output the first part of the assembly.
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// The first character of the equation must be a number.
	printf("  mov rax, %d\n", expect_number());

	// Output assembly as it consumes the tokens of `+ <num>` or `- <num>`.
	while (!at_eof()) {
		if (consume('+')){
			printf("  add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("  sub rax, %d\n", expect_number());
	}

	printf("  ret\n");
	return 0;
}