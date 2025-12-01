CXX      = g++
CXXFLAGS = -Wall -O2 -std=c++17 -Iinclude -Ilib

SRC_DIR  = system
OBJ_DIR  = build
BIN_DIR  = bin
TEST_DIR = test

# --- System sources ---
SRCS      = $(wildcard $(SRC_DIR)/*.cpp)
MAIN_SRC  = $(SRC_DIR)/main.cpp
LIB_SRCS  = $(filter-out $(MAIN_SRC),$(SRCS))

OBJS      = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
LIB_OBJS  = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LIB_SRCS))
MAIN_OBJ  = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRC))

TARGET    = $(BIN_DIR)/myprogram

# --- Test executables ---
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINS = $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/%,$(TEST_SRCS))

# --- Default target builds everything ---
all: $(TARGET) $(TEST_BINS)

# Build only tests
tests: $(TEST_BINS)

# Build only main program
main: $(TARGET)

# --- Link main program ---
$(TARGET): $(LIB_OBJS) $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LIB_OBJS) $(MAIN_OBJ) -o $@

# --- Compile system sources ---
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Build each test cpp into its own executable (linking with system library objects only) ---
$(BIN_DIR)/%: $(TEST_DIR)/%.cpp $(LIB_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --- Clean ---
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
