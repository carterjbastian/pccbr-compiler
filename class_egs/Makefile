# Makefile for making a simple C scanner.
# WARNING: it's not for the full C language...doesn't even do comments.

# Designed for scanner in C, caller in C


CC = gcc
FLEXFLAGS = -ll
# use -lfl for Linux; -ll for OSX

.SUFFIXES: .c

# Scanner and caller both in C
cscan: lex.yy.o cscan.o
	$(CC) -o $@ lex.yy.o cscan.o $(FLEXFLAGS)

.c.o:
	$(CC) -c $< 

lex.yy.c: cscan.l
	flex $<


clean:
	rm -f lex.yy.* *.o  cscan *~
