#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	int len;		// Token length
};

// AST node kind
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
  	ND_EQ,  // ==
  	ND_NE,  // !=
  	ND_LT,  // <
  	ND_LE,  // <=
	ND_NUM, // Integer
}NodeKind;

// AST node type
typedef struct Node Node;
struct Node{
	NodeKind kind;  // Node kind
	Node *lhs;		// left hand side
	Node *rhs;		// right hand side
	int val;		// used when kind is ND_NUM
};

//Input program
char *user_input;

// Token on the scope
Token *token;

// Tokenizer
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize();

// Copde generator
void gen(Node *node);

// Parser
Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();