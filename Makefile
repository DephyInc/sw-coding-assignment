CC=g++
CFLAGS= -std=c++17 -Wall -pedantic -g
SOURCES= main.cpp veprom.cpp
DEPS= veprom.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(EXECUTABLE)
	@echo Make has finished.

$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

clean: 
	$(RM) *.o *~ $(EXECUTABLE)
