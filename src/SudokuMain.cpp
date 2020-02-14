#include <chrono>
#include <ctime>
#include <cstdlib>
#include <stack>
#include <iterator>
#include <list>
#include <mutex>
#include <thread>

#include "Sudoku/SudokuEngine.h"
#include "Sudoku/SudokuBoard.h"
#include "Sudoku/SudokuGame.h"

using namespace wubinboardgames::sudoku;

int main(int argc, char ** argv)
{

#ifdef _testing
    SudokuBoard board;
    GenerateNewGame<SudokuBoard>(board);
#else
    DisplayOptionsMenu();
#endif
    return 0;
}