# Compiler and Linker
CXX = g++                  # C++ compiler
CXXFLAGS = -Wall -Wextra -O2 -fPIC  # Compiler flags for C++
LDFLAGS = -shared                  # Link as a shared library

# Directories
SRC_DIR = wdt_ct                  # Assuming source code is in the wdt_ct folder
BUILD_DIR = build                 # Directory for object files
INCLUDE_DIR = .                   # Include current directory for headers
LIBRARY_DIR = lib                 # Where to output the library

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)  # All .cpp files in the src directory
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)  # Object files
TARGET = $(LIBRARY_DIR)/libwdt_ss.so  # Final shared library file

# Create directories if they don't exist
$(shell mkdir -p $(BUILD_DIR) $(LIBRARY_DIR))

# Default target: Build the shared library
all: $(TARGET)

# Rule to build the shared library
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

# Rule to compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

# Clean build files
clean:
	rm -rf $(BUILD_DIR)/*.o $(LIBRARY_DIR)/*

# Install the shared library
install: $(TARGET)
	cp $(TARGET) /usr/local/lib
	ldconfig  # Update the linker cache

.PHONY: all clean install
