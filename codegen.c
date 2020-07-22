#include "zcc.h"
//
// Code generator
//

// Pushes the given node's address to the stack.
static void gen_addr(Node *node){
	if (node->kind == ND_VAR){
		int offset = (node->name - 'a' + 1) * 8;
		printf("  lea rax, [rbp-%d]\n", offset);
		printf("  push rax\n");
		return;
	}

	error("not an lvalue");
}

static void load(void){
	printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
}

static void store(void){
	printf("  pop rdi\n");
	printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
}

void gen(Node *node){
    switch (node->kind){
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
	case ND_EXPR_STMT:
		gen(node->lhs);
		printf("  add rsp, 8\n");
		return;
    case ND_VAR:
        gen_addr(node);
        load();
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        gen(node->rhs);
        store();
        return;
	case ND_RETURN:
		gen(node->lhs);
		printf("  pop rax\n");
		printf("  jmp .L.return\n");
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
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    
    }

    printf("  push rax\n");
}

void codegen(Node *node){
	// Output the first part of the assembly.
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // Prologue
    // Allocate 26 memories for variables
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    // Generate codes from the top of the stmt
    for (Node *n = node; n; n = n->next)
        gen(n);

    // Epilogue
    // return the result which is remaining on the top of the stack.
    printf(".L.return:\n");
	printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}