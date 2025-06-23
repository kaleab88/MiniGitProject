<<<<<<< HEAD
# Compiler
CXX = g++

# Standard C++ version
CXXSTD = c++17

# Compiler flags
CXXFLAGS = -std=$(CXXSTD) -Wall -Wextra -pedantic

# Linker flags for OpenSSL and filesystem
LDFLAGS = -lssl -lcrypto -lz -lstdc++fs
=======
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
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063

# Source files
SRCS = main.cpp minigit.cpp utils.cpp

<<<<<<< HEAD
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
=======
# Executable name
TARGET = minigit

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Optional: Add a clean rule for convenience
clean:
	rm -f $(TARGET)
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
