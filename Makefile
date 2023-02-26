.PHONY: default all test format clean coverage_reports

default:
	$(MAKE) -C src

all: default format doc coverage_reports

doc:
	doxygen Doxyfile

%_test_coverage/executable: src/*.c src/*.h test/*.cpp test/*.hpp
	cp -r test $*_test_coverage
	$(MAKE) -C $*_test_coverage clean
	sed -i 's/CXX_FLAGS = /CXX_FLAGS = -fprofile-arcs -ftest-coverage /g' $*_test_coverage/Makefile
	sed -i 's/LIBRARIES = /LIBRARIES = -lgcov /g' $*_test_coverage/Makefile
	$(MAKE) -C $*_test_coverage $*_test
	mv $*_test_coverage/$*_test $*_test_coverage/executable

%_test_coverage/coverage.info: %_test_coverage/executable
	$<
	lcov --capture --directory $*_test_coverage --output-file $@

%_test_coverage/report/index.html: %_test_coverage/coverage.info
	mkdir -p $*_test_coverage/report
	genhtml $< --output-directory $*_test_coverage/report

coverage_reports: $(patsubst test/%_test.cpp, %_test_coverage/report/index.html, $(wildcard test/*_test.cpp) test/all_test.cpp)
	bash summarise_coverage_reports.sh | column -t

format:
	$(MAKE) -C src format
	$(MAKE) -C test format

test:
	$(MAKE) -C test run

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	rm -rf *_test_coverage doc
