CC = gcc
CFLAGS = -g
BISONFL = -d -v
FLEXFLAGS = -ll

.PHONY: clean parser1 parser2a parser2b parser2c parser3 parser4 parser4bad parser5

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

parser1:
	cp -f parser.1.l parser.l
	cp -f parser.1.y parser.y
	make parser

parser2a:
	cp -f parser.2.l parser.l
	cp -f parser.2a.y parser.y
	make parser

parser2b:
	cp -f parser.2.l parser.l
	cp -f parser.2b.y parser.y
	make parser

parser2c:
	cp -f parser.2.l parser.l
	cp -f parser.2c.y parser.y
	make parser

parser3:
	cp -f parser.2.l parser.l
	cp -f parser.3.y parser.y
	make parser

parser4:
	cp -f parser.2.l parser.l
	cp -f parser.4.y parser.y
	make parser

parser4bad:
	cp -f parser.2.l parser.l
	cp -f parser.4.bad.y parser.y
	make parser

parser5:
	cp -f parser.5.l parser.l
	cp -f parser.5.y parser.y
	make parser

clean :
	rm -f parser lex.yy.o parser.tab.o ast_main.o ast.o \
	parser.tab.h parser.tab.c lex.yy.c *~ parser.output testing


depend :
	makedepend -- $(CFLAGS) -- ast_main.c

# DO NOT DELETE THIS LINE -- make depend depends on it.
