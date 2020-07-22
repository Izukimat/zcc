#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

typedef enum {
	TK_RESERVED, // Keywords or punctuators
	TK_IDENT,   // Identifier
	TK_NUM,		 // Integer literals
	TK_EOF,		 // EOF markers
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
	TokenKind kind;	// Token kind
	Token *next;	// next Token input
	long val;		// the value if TokenKind is TK_NUM
	char *str;		// Token strings
	int len;		// Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_indent(void);
void expect(char *op);
long expect_number(void);
bool at_eof(void);
Token *tokenize(void);

extern char *user_input;
extern Token *token;

//
// parse.c
//

typedef enum {
	ND_ADD, 	// +
	ND_SUB,		// -
	ND_MUL, 	// *
	ND_DIV, 	// /
  	ND_EQ,  	// ==
  	ND_NE,  	// !=
  	ND_LT,  	// <
  	ND_LE,  	// <=
	ND_NUM, 	// Integer
	ND_ASSIGN,	// =
	ND_VAR,	// local variable
	ND_RETURN,    // "return"
  	ND_EXPR_STMT, // Expression statement
}NodeKind;

// AST node type
typedef struct Node Node;
struct Node{
	NodeKind kind;  // Node kind
	Node *next;		// next node
	Node *lhs;		// left hand side
	Node *rhs;		// right hand side
	int val;		// used when kind is ND_NUM
	char name;		// used when kind is ND_VAL
};

Node *program(void);

//
// codegen.c
//
void codegen(Node *node);