# Makefile for veprom

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -g

# Source files
SRCS = veprom.cpp fileHandler.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = veprom

# Default target
all: $(TARGET)

# Rule for compiling .cpp files into .o object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for linking object files into the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Clean rule to remove object files and the target executable
clean:
	$(RM) $(OBJS) $(TARGET)
