#include "zcc.h"

//
// Tokenizer
//

char *user_input;
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
bool consume(char *op){
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        strncmp(token->str, op, token->len))
      return false;
    token = token->next;
    return true;
}

// Consumes the current token if it is an identifier.
Token *consume_indent(void){
    if (token->kind != TK_IDENT)
        return NULL;
    Token *t = token;
    token = token->next;
    return t;
}

// When the next token is the expected sign, move it one step forward.
// Otherwise, raise an error.
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      strncmp(token->str, op, token->len))
    error_at(token->str, "expected \"%s\"", op);
    token = token->next;
}

// When the next token is a numerical value, move it one step forward.
// Otherwise raise an error.
long expect_number(void) {
    if (token->kind != TK_NUM)
        error_at(token->str, "expected a number");
    long val = token->val;
    token = token->next;
    return val;
}

bool at_eof(void){
    return token->kind == TK_EOF;
}

// Create a new token and connect it to cur.
static Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

static bool startswith(char *p, char *q){
    return strncmp(p, q, strlen(q)) == 0;
}

static bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}

// Tokenize `user_input` and returns new tokens.
Token *tokenize(void){
    char *p = user_input;
    Token head = {};
    Token *cur = &head;

    while (*p){
        // Skip empty spaces.
        if (isspace(*p)){
            p++;
            continue;
        }

		// Keywords
		if (startswith(p, "return") && !is_alnum(p[6])){
			cur = new_token(TK_RESERVED, cur, p, 6);
			p += 6;
			continue;
		}

		// Identifier
        if ('a' <= *p && *p <= 'z'){
            cur = new_token(TK_IDENT, cur, p++, 1);
            continue;
        }

        // Multi-letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
            }

        // Single-letter punctuator
        if (ispunct(*p)){
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "invalid token");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}


