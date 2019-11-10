CC = gcc
OFLAGS = -Wall -Wextra
PROGS = program

all: $(PROGS)

program:list.o tree.o exec.o main.o
	$(CC) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(PROGS) *.o

run:
	rlwrap ./$(PROGS)
