CFLAGS = -std=c99 -W -Wall
LDLIBS =

.PHONY: all clean
all : regex

regex : regex.c

clean :
	$(RM) regex *.o output.*
