CXX = g++
CXXFLAGS = -Wall -std=c++17

# executable
TARGET = disk_build

# sources to compiles
SRCS = disk.cpp test_disk.cpp 

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)