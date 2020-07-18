#include "zcc.h"

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