#include "zcc.h"


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

Node *new_unary(NodeKind kind, Node *expr) {
  Node *node = new_node(kind);
  node->lhs = expr;
  return node;
}

Node *new_num(int val){
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *new_var_node(char name){
	Node *node = new_node(ND_VAR);
	node->name = name;
	return node;
}

/*
program    = stmt*
stmt       = expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
*/

Node *stmt(void);
Node *expr(void);
Node *assign(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

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
Node *stmt(void){
	if (consume("return")){
		Node *node = new_unary(ND_RETURN, expr());
		expect(";");
		return node;
	}

    Node *node = new_unary(ND_EXPR_STMT, expr());
	expect(";");
	return node;
}

// expr = assign
Node *expr(void) {
  return assign();
}

// assign equality ("=" assign)?
Node *assign(void) {
    Node *node = equality();
    if (consume("="))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;    
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(void) {
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
Node *relational(void) {
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
Node *add(void) {
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
Node *mul(void){
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
Node *unary(void){
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}

// primary = "(" expr ")" | ident | num
Node *primary(void){
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

