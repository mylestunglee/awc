TARGET = test
CXX = g++
CXX_FLAGS += -funsigned-char -g -std=c++2a -Wall -Wextra -Wno-missing-field-initializers -Wpedantic -Wshadow
LIBRARIES += -lglpk -lgtest -lgtest_main -lpthread

.PHONY: default run format clean

TEST_SOURCES = $(filter-out all_test.cpp, $(wildcard *_test.cpp))
TARGETS = $(patsubst %_test.cpp, %_test, $(TEST_SOURCES))
FIXTURE_OBJECTS = $(patsubst %_fixture.cpp, %_fixture.o, $(wildcard *_fixture.cpp))
TEST_OBJECTS = $(patsubst %_test.cpp, %_test.o, $(TEST_SOURCES))
SOURCE_OBJECTS = $(patsubst ../src/%.c, %.o, $(filter-out ../src/main.c, $(wildcard ../src/*.c)))
HEADERS = $(wildcard ../src/*.h)

.PRECIOUS: $(TARGETS) $(FIXTURE_OBJECTS) $(TEST_OBJECTS)

default: $(TARGETS) all_test

%.o: ../src/%.c $(HEADERS)
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

format:
	clang-format -style=file -i *.cpp *.hpp

clean:
	rm -f $(TARGETS) $(FIXTURE_OBJECTS) $(TEST_OBJECTS) all_test.cpp all_test state*.txt
