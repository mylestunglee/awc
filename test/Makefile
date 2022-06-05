TARGET = test
CXX = g++
CXX_FLAGS = -g -Wall -pedantic -funsigned-char -std=c++2a
LIBRARIES = -lgtest_main -lgtest -lglpk -lpthread

.PHONY: default clean run

TEST_SOURCES = $(filter-out all_test.cpp, $(wildcard *_test.cpp))
TARGETS = $(patsubst %_test.cpp, %_test, $(TEST_SOURCES)) all_test
FIXTURE_OBJECTS = $(patsubst %_fixture.cpp, %_fixture.o, $(wildcard *_fixture.cpp))
TEST_OBJECTS = $(patsubst %_test.cpp, %_test.o, $(TEST_SOURCES))
SOURCE_OBJECTS = $(patsubst ../%.c, %.o, $(filter-out ../main.c, $(wildcard ../*.c)))
HEADERS = $(wildcard ../*.h)

.PRECIOUS: $(TARGETS) $(FIXTURE_OBJECTS) $(TEST_OBJECTS)

%.o: ../%.c $(HEADERS)
	$(CXX) $(CXX_FLAGS) $(LIBRARIES) -c $< -o $@

all_test.cpp: $(TEST_SOURCES)
	rm -f $@
	grep -Eh "#define EXPOSE_[A-Z_]+_INTERNALS" $^ > $@
	for TEST_SOURCE in $^; \
	do \
		echo "#include \"$$TEST_SOURCE\"" >> $@; \
	done

%_test.o: %_test.cpp $(HEADERS)
	$(CXX) $(CXX_FLAGS) $(LIBRARIES) -c $< -o $@

%_fixture.o: %_fixture.cpp $(HEADERS)
	$(CXX) $(CXX_FLAGS) $(LIBRARIES) -c $< -o $@

%_test: %_test.o $(SOURCE_OBJECTS) $(FIXTURE_OBJECTS)
	$(CXX) $< $(SOURCE_OBJECTS) $(FIXTURE_OBJECTS) $(LIBRARIES) -o $@

run: all_test
	./$<

default: $(TARGETS)

clean:
	rm -f $(TARGETS) $(FIXTURE_OBJECTS) $(TEST_OBJECTS) all_test.cpp
