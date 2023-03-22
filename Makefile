# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++20

# Directories
SRC_DIR		:= ./src
TEST_DIR	:= ./tests
BUILD_DIR	:= ./build

# Executable build location
EXECUTABLE	:= $(BUILD_DIR)/unit_test.o

# Source files
MAIN_SRC	:= $(SRC_DIR)/main.cpp
TEST_SRC	:= $(TEST_DIR)/main.cpp

# Append new sources here
OBJECTS		:= 	\
				$(SRC_DIR)/debug/logger.cpp \
				$(SRC_DIR)/cartridge/cartridge.cpp \
				$(SRC_DIR)/cpu/Cpu.cpp \
				$(SRC_DIR)/cpu/Bus.cpp \
				$(SRC_DIR)/apu/Apu.cpp \
				$(SRC_DIR)/ppu/ppu.cpp \

.PHONY: all clean

all: $(MAIN_SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(MAIN_SRC) $(OBJECTS)

test: $(TEST_SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(TEST_SRC) $(OBJECTS)

clean:
	rm -f $(BUILD_DIR)/