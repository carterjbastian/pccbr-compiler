CC = gcc
CFLAGS = -g

buildtab.o : buildtab.c
	$(CC) $(CFLAGS) -c $< 

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -f *.o
