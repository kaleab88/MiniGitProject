# Compiler
CXX = g++

# Standard C++ version
CXXSTD = c++17

# Compiler flags
CXXFLAGS = -std=$(CXXSTD) -Wall -Wextra -pedantic

# Linker flags for OpenSSL and filesystem
LDFLAGS = -lssl -lcrypto -lz -lstdc++fs

# Source files
SRCS = main.cpp minigit.cpp utils.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = minigit

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf .minigit # Remove the .minigit directory for a clean slate

.PHONY: all clean