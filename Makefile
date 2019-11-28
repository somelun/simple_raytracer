CC=clang++
CFLAGS=-c -Wall -std=c++11 -O2
LDFLAGS=
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ptracer

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o all

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) all
