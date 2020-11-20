CC= gcc
CFLAGS= -Wall -Wextra -pedantic -ansi -g

BINARIES= task3.out
SOURCES= list.c io.c

OBJS=$(patsubst %.c, %.o, $(SOURCES))

all: $(BINARIES)

task3.out: main.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $^

list.c: list.h
io.c: io.h

clean:
	rm -f *.o

distclean: clean
	rm -f $(BINARIES)

