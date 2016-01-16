# Makefile for making a simple C scanner.
# WARNING: it's not for the full C language...doesn't even do comments.

# Designed for scanner in C, caller in C


CC = gcc
FLEXFLAGS = -ll
# use -lfl for Linux; -ll for OSX

TESTDIR = tests

.SUFFIXES: .c

# Scanner and caller both in C
cscan: lex.yy.o cscan.o
	$(CC) -o $@ lex.yy.o cscan.o $(FLEXFLAGS)

.c.o:
	$(CC) -c $< 

lex.yy.c: cscan.l
	flex $<

%.c : %.l

test: lex.yy.c cscan.c lex_tests.c
	$(CC) -o $@ -D _TEST_MODE_ lex.yy.c lex_tests.c cscan.c $(FLEXFLAGS)

# A second copy since you can't use the keyword test in a shell script
lextest: lex.yy.c cscan.c $(TESTDIR)/lex_tests.c $(TESTDIR)/lex_tests.h
	$(CC) -o $@ -D _TEST_MODE_ lex.yy.c $(TESTDIR)/lex_tests.c cscan.c $(FLEXFLAGS)


clean:
	rm -f lex.yy.* *.o  cscan *~ lextest
