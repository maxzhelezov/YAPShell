CC= gcc
CFLAGS= -Wall -Wextra -ansi -g

BINARIES= task5.out
SOURCES= list.c io.c tree.c exec.c

OBJS=$(patsubst %.c, %.o, $(SOURCES))

all: $(BINARIES)

task5.out: main.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $^

io.c: io.h
list.c: list.h
tree.c: tree.h
exec.c: exec.h

run:
	rlwrap ./$(BINARIES)

clean:
	rm -f *.o

distclean: clean
	rm -f $(BINARIES)

