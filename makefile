CC = gcc
CFLAGS = -Wall -O1
OBJS = command.o word.o my-shell.o symbol.o
SRCS = command.c word.c my-shell.c symbol.c
EXECUTABLE = my-shell

all: $(EXECUTABLE)

debug:
	$(CC) $(CFLAGS) -c -g -D DEBUG *.c
	$(CC) $(CFLAGS) -g -D DEBUG -o $(EXECUTABLE) $(OBJS)

test-list:
	$(CC) $(CFLAGS) -c -g command.c word.c symbol.c test-list.c
	$(CC) $(CFLAGS) -g -o test-list.out command.o word.o symbol.o test-list.o
	./test-list.out

clean:
	-rm *.o
	-rm $(EXECUTABLE)
	-rm ./test-list.out

$(OBJS):
	$(CC) $(CFLAGS) -c $(SRCS)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJS)


