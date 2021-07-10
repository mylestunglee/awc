TARGET = awc
CC = gcc
CFLAGS = -g -Wall -pedantic -Wunused-macros
LIBRARIES = -lglpk

default: $(TARGET)
all: default test doc

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBRARIES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARIES) -o $@

test:
	$(MAKE) -C test

runtest:
	$(MAKE) -C test run

format:
	clang-format -style=file -i *.c *.h test/*.cpp

doc:
	doxygen Doxyfile

clean:
	rm -f $(TARGET) $(OBJECTS)
	$(MAKE) -C test clean