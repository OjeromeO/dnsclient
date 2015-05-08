CC = gcc
LFLAGS = -g -W -Wall -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
CFLAGS = -g -W -Wall -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = dnsclient



all : $(EXEC)

dnsclient : $(OBJ)
	$(CC) $(LFLAGS) -o $@ $^
dnsclient.o : dnsclient.c toolbox.h arguments.h queries.h responses.h socket.h structures.h
	$(CC) -c $(CFLAGS) $<
toolbox.o : toolbox.c toolbox.h global.h
	$(CC) -c $(CFLAGS) $<
arguments.o : arguments.c arguments.h global.h
	$(CC) -c $(CFLAGS) $<
queries.o : queries.c queries.h structures.h global.h
	$(CC) -c $(CFLAGS) $<
responses.o : responses.c responses.h global.h structures.h
	$(CC) -c $(CFLAGS) $<
socket.o : socket.c socket.h global.h
	$(CC) -c $(CFLAGS) $<

doxydoc :
	doxygen Doxyfile

clean :
	rm -f $(OBJ)
mrproper : clean
	rm -f $(EXEC)


