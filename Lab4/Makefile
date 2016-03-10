# CS 111 Lab 4 Makefile

CC = gcc
CFLAGS = -pthread

all: addtest

ADDTEST_SOURCES = addtest.c
ADDTEST_OBJECTS = $(subst .c,.o,$(ADDTEST_SOURCES))

addtest: $(ADDTEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(ADDTEST_OBJECTS) -lrt

clean:
	rm -fr *.o *~ *.bak *.tar.gz core *.core *.tmp addtest $(DISTDIR)
