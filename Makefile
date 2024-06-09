CXX = clang++
SRC_PATH = src
BIN_NAME = main


SRC_EXT = cpp
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)

COMPILE_FLAGS = -std=c++17 -Wall
MAIN = cmd/main.cpp
all:
	@echo "Compliing $< -> $@"
	$(CXX) $(COMPILE_FLAGS) $(SOURCES) $(MAIN) -o $(BIN_NAME)
	./${BIN_NAME}
