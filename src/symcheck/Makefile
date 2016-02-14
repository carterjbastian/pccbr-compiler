CC = gcc
CFLAGS = -g

all: symcheck

symcheck: ../util/ast.o ../util/symtab.o ../symtab/buildtab.o ../parser/parser.tab.o ../lexer/cscan.yy.o tree_check.o symbols_main.o
	$(CC) -o $@ $(CFLAGS) ../lexer/cscan.yy.o ../symtab/buildtab.o ../parser/parser.tab.o ../util/ast.o ../util/symtab.o tree_check.o symbols_main.o $(FLEXFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

../util/ast.o : ../util/ast.c
	cd ../util; make ast.o

../util/symtab.o : ../util/symtab.c
	cd ../util; make symtab.o

../symtab/buildtab.o :
	cd ../symtab; make

../parser/parser.tab.o:
	cd ../parser; make $(@F)

../lexer/cscan.yy.o:
	cd ../lexer; make complete

clean :
	rm -f *.o symcheck
