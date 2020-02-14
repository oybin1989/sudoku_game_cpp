
OS := $(shell uname)

ifeq ($(OS),Linux)
 CC = g++
else
 CC = clang++
endif


all: sudoku

sudoku:
	$(CC) -O2 -static-libstdc++ -static-libgcc -o bin/sudoku --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp

sudoku_testing:
	$(CC) -O2 -D_testing -static-libstdc++ -static-libgcc -o bin/sudoku --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp
	bin/sudoku

sudoku_debug:
	$(CC) -g -static-libstdc++ -static-libgcc -o bin/sudoku --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp