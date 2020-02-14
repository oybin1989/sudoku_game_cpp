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