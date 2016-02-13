CC = gcc
CFLAGS = -g
BISONFL = -d -v
FLEXFLAGS = -ll

.PHONY: clean 

.SUFFIXES: .c

.cc.o:
	$(CC) $(CFLAGS) -c $<

symtab : ../lexer/cscan.yy.o parser.tab.o ast.o symtab.o symbols_main.o tree_check.o
	$(CC) -ggdb -o $@ $(CFLAGS) ../lexer/cscan.yy.o parser.tab.o ast.o symtab.o tree_check.o symbols_main.o $(FLEXFLAGS)

parse : ../lexer/cscan.yy.o parser.tab.o ast.o ast_main.o
	$(CC) -o $@ $(CFLAGS) ../lexer/cscan.yy.o parser.tab.o ast.o ast_main.o $(FLEXFLAGS)

test : ../lexer/cscan.yy.o parser.tab.o ast.o symtab.o symbols_main.o
	$(CC) -c $(CFLAGS) -D _TESTING_ symbols_main.c
	$(CC) -o $@ $(CFLAGS) -D _TESTING_ ../lexer/cscan.yy.o parser.tab.o ast.o symtab.o symbols_main.o $(FLEXFLAGS)

../lexer/cscan.yy.o : ../lexer/cscan.l
	cd ../lexer; make complete

parser.tab.o : parser.tab.c
	$(CC) -c $(CFLAGS) $<

parser.tab.h : parser.y
	bison $(BISONFL) $<

parser.tab.c : parser.y
	bison $(BISONFL) $<

ast_main.o : ast_main.c
	$(CC) -c $(CFLAGS) $<

symbols_main.o : symbols_main.c
	$(CC) -c $(CFLAGS) symbols_main.c

symtab.o : symtab.c symtab.h
	$(CC) -c $(CFLAGS) symtab.c

clean :
	rm -f parser lex.yy.o parser.tab.o tree_main.o ast.o \
	parser.tab.h parser.tab.c lex.yy.c *~ parser.output test \
    	symtab.o symtab *.o parse

depend :
	makedepend -- $(CFLAGS) -- ast_main.c

# DO NOT DELETE THIS LINE -- make depend depends on it.
