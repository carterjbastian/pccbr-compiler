CC = gcc
CFLAGS = -g
FLEXFLAGS = -ll

all: quadgen

quadgen: ../util/ast.o ../util/symtab.o ../symtab/buildtab.o ../parser/parser.tab.o ../lexer/cscan.yy.o quadgen.o quadgen_main.o
	$(CC) -o $@ $(CFLAGS) ../lexer/cscan.yy.o ../symtab/buildtab.o ../parser/parser.tab.o ../util/ast.o ../util/symtab.o quadgen.o quadgen_main.o $(FLEXFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

../util/ast.o : ../util/ast.c
	cd ../util; make ast.o

../util/symtab.o : ../util/symtab.c
	cd ../util; make symtab.o

../symtab/buildtab.o :
	cd ../symtab; make

../parser/parser.tab.o:
	cd ../parser; make parser.tab.o

../lexer/cscan.yy.o:
	cd ../lexer; make complete

clean :
	rm -f *.o quadgen
	cd ../symcheck; make clean
