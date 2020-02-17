
OS := $(shell uname)

ifeq ($(OS),Linux)
 CC = g++
else
 CC = clang++
endif

all: cell_test board_test engine_test

board_test:
	$(CC) --std=c++14 -o board_test sudoku_board_test.cpp -I.. -I../includes -I../includes/Sudoku -L../libs -lgtest -lpthread
	./board_test
	rm -f board_test

cell_test:
	$(CC) --std=c++14 -o cell_test sudoku_cell_test.cpp -I.. -I../includes -I../includes/Sudoku -L../libs -lgtest -lpthread
	./cell_test
	rm -f cell_test

engine_test:
	$(CC) --std=c++14 -o engine_test sudoku_engine_test.cpp -I.. -I../includes -I../includes/Sudoku -L../libs -lgtest -lpthread
	./engine_test
	rm -f engine_test