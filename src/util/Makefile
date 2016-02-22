CC = gcc
CFLAGS = -g

ast.o : ast.c
	$(CC) $(CFLAGS) -c $<

symtab.o : symtab.c
	$(CC) $(CFLAGS) -c $<

unescape.o : unescape.c
	$(CC) $(CFLAGS) -c $<

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -f *.o
