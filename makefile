CC=gcc
CFLAGS=-I.
DEPS = headers.h
OBJ = main.o pwd.o echo.o cd.o parse.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f shell *.o