TARGET = awc
CC = gcc
CFLAGS = -g -std=c17 -Wall -Wextra -Wpedantic -Wunused-macros -Wstrict-prototypes -Wshadow
LIBRARIES = -lglpk
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBRARIES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)
.PHONY: default clean test run_test clean_test coverage_reports

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARIES) -o $@

default: $(TARGET)

format:
	clang-format -style=file -i *.c *.h

tidy:
	clang-tidy -header-filter=.* -system-headers -checks=-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling *.c -fix --

doc:
	doxygen Doxyfile

clean:
	rm -rf $(TARGET) $(OBJECTS)

