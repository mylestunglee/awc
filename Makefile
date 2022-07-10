TARGET = awc
CC = gcc
CFLAGS = -g -Wall -Wextra -Wunused-macros
LIBRARIES = -lglpk
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)
REPORTS_DIRECTORY = reports

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBRARIES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)
.PHONY: default clean test run_test clean_test coverage_reports

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARIES) -o $@

default: $(TARGET)

test:
	$(MAKE) -C test

format:
	clang-format -style=file -i *.c *.h test/*.cpp

tidy:
	clang-tidy -header-filter=.* -system-headers -checks=-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling *.c -fix --

doc:
	doxygen Doxyfile

clean: clean_test
	rm -rf $(TARGET) $(OBJECTS) $(REPORTS_DIRECTORY)

run_test:
	$(MAKE) -C test run

clean_test:
	$(MAKE) -C test clean

%_test_coverage: clean_test
	cp -r test $@

%_test_coverage/Makefile: %_test_coverage
	sed -i 's/CXX_FLAGS = /CXX_FLAGS = -fprofile-arcs -ftest-coverage /g' $@
	sed -i 's/LIBRARIES = /LIBRARIES = -lgcov /g' $@

%_test_coverage/executable: %_test_coverage/Makefile
	$(MAKE) -C $*_test_coverage $*_test
	mv $*_test_coverage/$*_test $*_test_coverage/executable

%_test_coverage/coverage.info: %_test_coverage/executable
	$<
	lcov --capture --directory $*_test_coverage --output-file $@

%_test_coverage/report: %_test_coverage
	mkdir -p $@

%_test_coverage/report/index.html: %_test_coverage/coverage.info %_test_coverage/report
	genhtml $< --output-directory $*_test_coverage/report

$(REPORTS_DIRECTORY):
	mkdir -p $(REPORTS_DIRECTORY)

$(REPORTS_DIRECTORY)/%_coverage: $(REPORTS_DIRECTORY) %_test_coverage/report/index.html
	mv $*_test_coverage/report $@
	rm -r $*_test_coverage

COVERAGE_REPORTS = $(patsubst test/%_test.cpp, $(REPORTS_DIRECTORY)/%_coverage, $(wildcard test/*_test.cpp) test/all_test.cpp) 

coverage_reports: $(COVERAGE_REPORTS)
