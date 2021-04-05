CC = gcc
CFLAGS = -Wall -O1
OBJS = command.o word.o my-shell.o
SRCS = command.c word.c my-shell.c
EXECUTABLE = my-shell

all: $(EXECUTABLE)

debug:
	$(CC) $(CFLAGS) -c -g *.c
	$(CC) $(CFLAGS) -g -o $(EXECUTABLE) $(OBJS)

test-list:
	$(CC) $(CFLAGS) -c -g command.c word.c test-list.c
	$(CC) $(CFLAGS) -g -o test-list.out command.o word.o test-list.o
	./test-list.out

clean:
	-rm *.o
	-rm $(EXECUTABLE)
	-rm ./test-list.out

$(OBJS):
	$(CC) $(CFLAGS) -c $(SRCS)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJS)


