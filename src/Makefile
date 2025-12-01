CXX = g++
CXXFLAGS = -Wall -std=c++17

# Build and source directories
BUILD_DIR = ../build
TARGET = $(BUILD_DIR)/my_program

# Automatically find all .o files in build/ and subdirs
OBJS = $(wildcard $(BUILD_DIR)/**/*.o) $(wildcard $(BUILD_DIR)/*.o)

# Default target
all: submodules $(TARGET)

# Call Makefiles in subdirectories to build object files
submodules:
	@$(MAKE) -C disk

# Link all object files into final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Clean everything
clean:
	@$(MAKE) -C disk clean
	rm -f $(TARGET)