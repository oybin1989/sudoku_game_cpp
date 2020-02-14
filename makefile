
OS := $(shell uname)

ifeq ($(OS),Linux)
 CC = g++
 STATIC_LINK = -static-libstdc++ -static-libgcc
else
 CC = clang++
 STATIC_LINK = 
endif


all: sudoku

sudoku:
	$(CC) -O2 $(STATIC_LINK) -o bin/sudoku --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp

sudoku_testing:
	$(CC) -O2 $(STATIC_LINK) -D_testing -o bin/sudoku --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp
	bin/sudoku

sudoku_debug:
	$(CC) -g $(STATIC_LINK) -o bin/sudoku --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp