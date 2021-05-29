TARGET = test
CXX = g++
CXXFLAGS = -g -Wall -pedantic -funsigned-char
LIBRARIES = -lgtest_main  -lgtest -lpthread

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LIBRARIES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBRARIES) -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
