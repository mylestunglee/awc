TARGET = awc
CC = gcc
CFLAGS = -g -Wall -pedantic
LIBRARIES = -lglpk

.PHONY: default all clean test

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBRARIES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARIES) -o $@

clean:
	rm $(TARGET) $(OBJECTS)
