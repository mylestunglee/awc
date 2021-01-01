TARGET = awc
CC = gcc
CFLAGS = -g -Wall -pedantic
LIBRARIES = -lglpk

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBRARIES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARIES) -Wall -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
