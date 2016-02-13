# Makefile for making a simple C scanner.
# WARNING: it's not for the full C language...doesn't even do comments.

# Designed for scanner in C, caller in C


CC = gcc
FLEXFLAGS = -ll
COMPLETEFLAGS = -D FULL_BUILD_
# use -lfl for Linux; -ll for OSX

TESTDIR = tests

.SUFFIXES: .c

# Scanner and caller both in C
cscan: lex.yy.o cscan.o
	$(CC) -o $@ lex.yy.o cscan.o $(FLEXFLAGS)

complete: cscan.yy.o

.c.o:
	$(CC) -c $< 

lex.yy.c: cscan.l
	flex $<

cscan.yy.c: cscan.l
	flex -o $@ $<

cscan.yy.o: cscan.yy.c
	$(CC) -c -o $@ $(COMPLETEFLAGS) $<

# A second copy since you can't use the keyword test in a shell script
lextest: lex.yy.c cscan_main.c $(TESTDIR)/lex_tests.c $(TESTDIR)/lex_tests.h
	$(CC) -o $@ -D TEST_MODE_ lex.yy.c $(TESTDIR)/lex_tests.c cscan_main.c $(FLEXFLAGS)

clean:
	rm -f lex.yy.* cscan.yy.* *.o  cscan *~ lextest
