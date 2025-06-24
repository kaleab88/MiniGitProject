# --- RESOLVED Makefile ---

# Compiler
CXX = g++

# Standard C++ version
CXXSTD = c++17

# Compiler flags
# -std=c++17 is needed for std::filesystem
# -Wall enables all warnings
# -Wextra enables extra warnings
# -pedantic enforces strict C++ standard compliance
CXXFLAGS = -std=$(CXXSTD) -Wall -Wextra -pedantic

# Linker flags for OpenSSL, Zlib, and filesystem
LDFLAGS = -lssl -lcrypto -lz -lstdc++fs

# Source files
SRCS = main.cpp minigit.cpp utils.cpp

# Object files (generated from source files)
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = minigit

# Default target: builds the executable
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile each .cpp file into a .o object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule: removes all generated object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf .minigit # Also remove the .minigit directory for a clean repository state

.PHONY: all clean