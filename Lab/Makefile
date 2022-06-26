flex: Lab.l
	flex --header-file=lexer.h -o lexer.c Lab.l
bison: Lab.y
	bison Lab.y --defines=grammar.h -o grammar.c
all: main.c flex bison
	gcc ast.c grammar.c lexer.c main.c -o main -g
clean: 
	rm grammar.c grammar.h lexer.c lexer.h main
