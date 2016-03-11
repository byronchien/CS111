# CS 111 Lab 4 Makefile

CC = gcc
CFLAGS = -pthread

all: sltest

sltest_SOURCES = SortedList.c sltest.c SortedList_m.c SortedList_s.c
sltest_OBJECTS = $(subst .c,.o,$(sltest_SOURCES))

sltest: $(sltest_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(sltest_OBJECTS) -lrt

clean:
	rm -fr *.o *~ *.bak *.tar.gz core *.core *.tmp sltest $(DISTDIR)
