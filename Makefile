CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iinclude

SRC_DIR := src
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.cc)

.PHONY: test clean

test: $(SRCS) test/main.cc
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) test/main.cc $(SRCS) -o $(BIN_DIR)/main && ./$(BIN_DIR)/main

clean:
	@find $(BIN_DIR) -mindepth 1 ! -name '.keep' -delete

.DEFAULT_GOAL := test
