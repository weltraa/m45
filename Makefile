CXX      		:= g++

CXXFLAGS 		:= -std=c++20 -Wall -Wextra -Iinclude
DEBUGFLAGS	:= -g

SRC_DIR := src
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.cc)

.PHONY: debug test clean

debug: $(SRCS) test/main.cc
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) test/main.cc $(SRCS) -o $(BIN_DIR)/main

clean:
	@find $(BIN_DIR) -mindepth 1 ! -name '.keep' -delete

.DEFAULT_GOAL := debug
