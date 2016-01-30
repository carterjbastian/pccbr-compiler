CC = gcc
CFLAGS = -g
BISONFL = -d -v
FLEXFLAGS = -ll

.PHONY: clean 

.SUFFIXES: .c

.cc.o:
	$(CC) $(CFLAGS) -c $<

parser : lex.yy.o parser.tab.o ast.o ast_main.o
	$(CC) -o $@ $(CFLAGS) lex.yy.o parser.tab.o ast.o ast_main.o $(FLEXFLAGS)

test : lex.yy.o parser.tab.o ast.o ast_main.o
	$(CC) -c $(CFLAGS) -D _TESTING_ ast_main.c
	$(CC) -o $@ $(CFLAGS) -D _TESTING_ lex.yy.o parser.tab.o ast.o ast_main.o $(FLEXFLAGS)

lex.yy.o : lex.yy.c
	$(CC) -c $(CFLAGS) $<

parser.tab.o : parser.tab.c
	$(CC) -c $(CFLAGS) $<

lex.yy.c : cscan.l parser.tab.h
	flex cscan.l

parser.tab.h : parser.y
	bison $(BISONFL) $<

parser.tab.c : parser.y
	bison $(BISONFL) $<

ast_main.o : ast_main.c
	$(CC) -c $(CFLAGS) $<

clean :
	rm -f parser lex.yy.o parser.tab.o ast_main.o ast.o \
	parser.tab.h parser.tab.c lex.yy.c *~ parser.output test


depend :
	makedepend -- $(CFLAGS) -- ast_main.c

# DO NOT DELETE THIS LINE -- make depend depends on it.
