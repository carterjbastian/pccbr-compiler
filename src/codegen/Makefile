CC = gcc
CFLAGS = -g
FLEXFLAGS = -ll

all: codegen

codegen: ../util/ast.o ../util/symtab.o ../util/unescape.o ../symtab/buildtab.o ../parser/parser.tab.o ../lexer/cscan.yy.o ../quadgen/quadgen.o codegen.o codegen_main.o ../symcheck/tree_check.o
	$(CC) -o $@ $(CFLAGS) ../lexer/cscan.yy.o ../symtab/buildtab.o ../parser/parser.tab.o ../util/ast.o ../util/symtab.o ../util/unescape.o ../quadgen/quadgen.o codegen.o codegen_main.o  $(FLEXFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

../util/ast.o : ../util/ast.c
	cd ../util; make ast.o

../util/symtab.o : ../util/symtab.c
	cd ../util; make symtab.o

../util/unescape.o : ../util/unescape.c
	cd ../util; make unescape.o

../symtab/buildtab.o :
	cd ../symtab; make

../symcheck/tree_check.o : ../symcheck/tree_check.c
	cd ../symcheck; make

../parser/parser.tab.o:
	cd ../parser; make parser.tab.o

../lexer/cscan.yy.o:
	cd ../lexer; make complete

../quadgen/quadgen.o:
	cd ../quadgen; make

clean :
	rm -f *.o codegen
	cd ../symcheck; make clean
	cd ../quadgen; make clean
	rm -f *.ys *.yo
