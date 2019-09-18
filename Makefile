CC		= gcc
CFLAGS		= -g
TARGET		= logParse
OBJECTS		= logParse.o
.SUFFIXES: .c .o

$(TARGET): $(OBJECTS)
	$(CC) -Wall -o $@ $(OBJECTS)

.c.o: logParse.c
	$(CC) -Wall $(CFLAGS) -c $<

.PHONY: clean
clean:
	/bin/rm -f *.o $(TARGET)
