TARGET = awc
CC = gcc
CFLAGS += -std=c17 -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wunused-macros
LIBRARIES = -lglpk
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBRARIES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)
.PHONY: default format tidy clean

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARIES) -o $@

default: $(TARGET)

format:
	clang-format -style=file -i *.c *.h

tidy:
	clang-tidy -header-filter=.* -system-headers -checks=-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling *.c -fix --

clean:
	rm -rf $(TARGET) $(OBJECTS)

