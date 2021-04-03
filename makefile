CC = gcc
CFLAGS = -Wall -O1
EXECUTABLE = my-shell

all: $(EXECUTABLE)

debug:
	$(CC) $(CFLAGS) -g -o $(EXECUTABLE) my-shell.c

clean:
	-rm $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) $(CFLAGS) -o $(EXECUTABLE) my-shell.c


