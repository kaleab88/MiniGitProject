# Makefile for MiniGit

# Compiler
CXX = g++

# Compiler flags
# -std=c++17 is needed for std::filesystem
# -Wall enables all warnings
# -g enables debugging information
CXXFLAGS = -std=c++17 -Wall -g

# Linker flags for OpenSSL and Zlib
LDFLAGS = -lssl -lcrypto -lz

# Source files
SRCS = main.cpp minigit.cpp utils.cpp

# Executable name
TARGET = minigit

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Optional: Add a clean rule for convenience
clean:
	rm -f $(TARGET)