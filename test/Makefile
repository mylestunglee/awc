TARGET = test
CXX = g++
CXXFLAGS = -g -Wall -pedantic -funsigned-char -std=c++2a
LIBRARIES = -lgtest_main  -lgtest -lglpk

.PHONY: default all clean

TARGETS = $(patsubst %.cpp, %.test, $(wildcard *.cpp))
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

.PRECIOUS: $(TARGETS) $(OBJECTS)

default: $(TARGETS)
all: default

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LIBRARIES) -c $< -o $@

%.test: %.o
	$(CXX) $< $(LIBRARIES) -o $@

clean:
	rm -f $(TARGETS) $(OBJECTS)
