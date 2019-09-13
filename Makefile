CC		= gcc
CFLAGS		= -g
TARGET		= logParse
OBJECTS		= logParse.o
.SUFFIXES: .c .o

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

.c.o: logParse.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	/bin/rm -f *.o $(TARGET)
