#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Tokenizer
//

// Token kind
typedef enum {
	TK_RESERVED, // Keywords or punctuators
	TK_NUM,		 // Integer literals
	TK_EOF,		 // EOF markers
} TokenKind;

// Token type
typedef struct Token Token;
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

		if (strchr("+-*/()", *p)){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p, "invalid token");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}

//
// Parser
//

// AST node kind
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // int
}NodeKind;

// AST node type
typedef struct Node Node;
struct Node{
	NodeKind kind;  // Node kind
	Node *lhs;		// left hand side
	Node *rhs;		// right hand side
	int val;		// used when kind is ND_NUM
};

Node *new_node(NodeKind kind){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node *expr();
Node *mul();
Node *unary();
Node *primary();

// expr = mul ("+" mul | "-" mul)*
Node *expr(){
	Node *node = mul();

	for (;;){
		if (consume('+'))
			node = new_binary(ND_ADD, node, mul());
		else if (consume('-'))
			node = new_binary(ND_SUB, node, mul());
		else
			return node;
	}
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(){
	Node *node = unary();

	for(;;){
		if (consume('*'))
			node = new_binary(ND_MUL, node, unary());
		else if (consume('/'))
			node = new_binary(ND_DIV, node, unary());
		else
			return node;
	}
}

// unary = ("+" | "-")? unary
//       | primary
Node *unary(){
	if (consume('+'))
		return primary();
	if (consume('-'))
		return new_binary(ND_SUB, new_num(0), unary());
	return primary();
}

// primary = "(" expr ")" | num
Node *primary(){
	// if the next token is "(", it should be "(" expr ")".
	if (consume('(')){
		Node *node = expr();
		expect(')');
		return node;
	}

	// otherwise it should be a digit
	return new_num(expect_number());
}

//
// Copde generator
//

void gen(Node *node){
	if (node->kind == ND_NUM){
		printf("  push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->kind){
	case ND_ADD:
		printf("  add rax, rdi\n");
		break;
	case ND_SUB:
		printf("  sub rax, rdi\n");
		break;
	case ND_MUL:
		printf("  imul rax, rdi\n");
		break;
	case ND_DIV:
		printf("  cqo\n");
		printf("  idiv rdi\n");
		break;
	}

	printf("  push rax\n");
}

int main(int argc, char **argv){
	if (argc != 2)
		fprintf(stderr, "Wrong number of input.");

	// Tokenize and parse
	user_input = argv[1];
	token = tokenize();
	Node *node = expr();

	// Output the first part of the assembly.
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// Traverse the AST to emit assembly.
	gen(node);

	// return the result which is remaining on the top of the stack.
	printf("  pop rax\n");
	printf("  ret\n");
	return 0;
}