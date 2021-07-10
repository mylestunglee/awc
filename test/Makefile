TARGET = test
CXX = g++
CXXFLAGS = -g -Wall -pedantic -funsigned-char -std=c++2a
LIBRARIES = -lgtest_main -lgtest -lglpk

.PHONY: default clean run

TARGETS = $(patsubst %_test.cpp, %_test, $(wildcard *_test.cpp))
FIXTURE_OBJECTS = $(patsubst %_fixture.cpp, %_fixture.o, $(wildcard *_fixture.cpp))
TEST_OBJECTS = $(patsubst %_test.cpp, %_test.o, $(wildcard *_test.cpp))
SOURCE_OBJECTS = $(patsubst ../%.c, %.o, $(filter-out ../main.c, $(wildcard ../*.c)))
HEADERS = $(wildcard ../*.h)

.PRECIOUS: $(TARGETS) $(FIXTURE_OBJECTS) $(TEST_OBJECTS) $(SOURCE_OBJECTS)

# build source objects
%.o: ../%.c $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LIBRARIES) -c $< -o $@

# build test objects
%_test.o: %_test.cpp
	$(CXX) $(CXXFLAGS) $(LIBRARIES) -c $< -o $@	

# build fixture objects
%_fixture.o: %_fixture.cpp
	$(CXX) $(CXXFLAGS) $(LIBRARIES) -c $< -o $@	

# build test executables
%_test: %_test.o $(SOURCE_OBJECTS) $(FIXTURE_OBJECTS)
	$(CXX) $< $(SOURCE_OBJECTS) $(FIXTURE_OBJECTS) $(LIBRARIES) -o $@

default: $(TARGETS)

run: $(TARGETS)
	@bash run_tests.sh $(TARGETS)

clean:
	rm -f $(TARGETS) $(FIXTURE_OBJECTS) $(TEST_OBJECTS) $(SOURCE_OBJECTS)
