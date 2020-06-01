#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Types of tokens
typedef enum {
	TK_RESERVED, // Keywords or punctuators
	TK_NUM,		 // Integer literals
	TK_EOF,		 // EOF markers
} TokenKind;

typedef struct Token Token;

// Token type
struct Token {
	TokenKind kind;	// Token kind
	Token *next;	// next Token input
	int val;		// the value if TokenKind is TK_NUM
	char *str;		// Token strings
};

//Input program
char *user_input;

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

// Reprots an error location and exit.
void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, ""); //print pos spaces.
	fprintf(stderr, "^ ");
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
		error_at(token->str, "expected '%c'", op);
	token = token->next;
}

// When the next token is a numerical value, move it one step forward.
// Otherwise raise an error.
int expect_number() {
	if (token->kind != TK_NUM)
		error_at(token->str, "expected a number");
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

// Tokenize `user_input` and returns new tokens.
Token *tokenize(){
	char *p = user_input;
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

		error_at(p, "expected a number");
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
	user_input = argv[1];
	token = tokenize();

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