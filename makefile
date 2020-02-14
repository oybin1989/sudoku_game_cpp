
OS := $(shell uname)

ifeq ($(OS),Linux)
 CC = g++
 STATIC_LINK = -static-libstdc++ -static-libgcc
 WARNING_OPTIONS = -Wall \
				   -pedantic \
				   -Wall -Wextra \
				   -Wcast-align \
				   -Wcast-qual \
				   -Wctor-dtor-privacy \
				   -Wdisabled-optimization \
				   -Wformat=2 -Winit-self \
				   -Wlogical-op \
				   -Wmissing-declarations \
				   -Wmissing-include-dirs \
				   -Wnoexcept -Wold-style-cast \
				   -Woverloaded-virtual \
				   -Wredundant-decls \
				   -Wshadow -Wsign-promo \
				   -Wstrict-null-sentinel \
				   -Wstrict-overflow=5 \
				   -Wswitch-default \
				   -Wundef \
				   -Werror \
				   -Wno-unused
else
 CC = clang++
 STATIC_LINK =
 WARNING_OPTIONS = -Wall
endif

all: sudoku

sudoku:
	$(CC) -O2 -o bin/sudoku_$(OS) --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp

sudoku_static:
	$(CC) -O2 $(STATIC_LINK) -o bin/sudoku_static_$(OS) --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp


sudoku_diagnose:
	$(CC) -O2 $(WARNING_OPTIONS) -o bin/sudoku_diagnose_$(OS) --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp

sudoku_testing:
	$(CC) -O2 -D_testing -o bin/sudoku_testing_$(OS) --std=c++11 -I./includes -pthread src/SudokuMain.cpp src/SudokuGame.cpp
	bin/sudoku_testing_$(OS)

sudoku_debug:
	$(CC) -g  $(WARNING_OPTIONS) -o bin/sudoku_debug_$(OS) --std=c++11 -I./includes src/SudokuMain.cpp src/SudokuGame.cpp -lpthread
