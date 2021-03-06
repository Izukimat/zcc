#include "zcc.h"

/*
program    = stmt*
stmt       = expr ";" | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
*/

static Node *stmt(void);
static Node *expr(void);
static Node *assign(void);
static Node *equality(void);
static Node *relational(void);
static Node *add(void);
static Node *mul(void);
static Node *unary(void);
static Node *primary(void);

static Node *new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_unary(NodeKind kind, Node *expr) {
  Node *node = new_node(kind);
  node->lhs = expr;
  return node;
}

static Node *new_num(int val){
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

static Node *new_var_node(char name){
	Node *node = new_node(ND_VAR);
	node->name = name;
	return node;
}

static Node *read_expr_stmt(void){
  return new_unary(ND_EXPR_STMT, expr());
}

// program = stmt*
Node *program(void){
    Node head = {};
  	Node *cur = &head;

  	while (!at_eof()) {
    	cur->next = stmt();
    	cur = cur->next;
  	}
  	return head.next;
}

// stmt = expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | ...
Node *stmt(void){
	if (consume("return")){
		Node *node = new_unary(ND_RETURN, expr());
		expect(";");
		return node;
	}
  
  if (consume("if")){
    Node *node = new_node(ND_IF);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else"))
      node->els = stmt();
    return node;
  }

  // statement with ;
  Node *node = read_expr_stmt();
	expect(";");
	return node;


}
// expr = assign
static Node *expr(void) {
  return assign();
}

// assign equality ("=" assign)?
static Node *assign(void) {
    Node *node = equality();
    if (consume("="))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;    
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(void) {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational(void) {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_binary(ND_LT, add(), node);
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
static Node *add(void) {
    Node *node = mul();

    for (;;){
        if (consume("+"))
            node = new_binary(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul(void){
    Node *node = unary();

    for(;;){
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? unary
//       | primary
static Node *unary(void){
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}

// primary = "(" expr ")" | ident | num
static Node *primary(void){
    // if the next token is "(", it should be "(" expr ")".
    if (consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_indent();
    if (tok)
        return new_var_node(*tok->str);
			
    // otherwise it should be a digit
    return new_num(expect_number());
}

