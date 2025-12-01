CXX = g++
CXXFLAGS = -Wall -std=c++17

BUILD_DIR = ../../build/heap
SRCS = page.cpp heapPage.cpp
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR) $(OBJS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o
