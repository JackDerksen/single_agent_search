CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Iinclude

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Executable name (.exe for Windows)
ifeq ($(OS),Windows_NT)
    TARGET := $(BIN_DIR)/hello.exe
    MKDIR_P := powershell -Command "New-Item -ItemType Directory -Force"
    RM_RF := powershell -Command "Remove-Item -Recurse -Force"
else
    TARGET := $(BIN_DIR)/hello
    MKDIR_P := mkdir -p
    RM_RF := rm -rf
endif

# Default
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	@$(MKDIR_P) $(BIN_DIR)
	$(CXX) $(OBJ) -o $@

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR_P) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM_RF) $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
