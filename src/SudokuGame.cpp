#include <iostream>
#include <cstdlib>

#include "Sudoku/SudokuEngine.h"
#include "Sudoku/SudokuBoard.h"
#include "Sudoku/SudokuGame.h"

using namespace wubinboardgames::sudoku;

namespace wubinboardgames
{
  namespace sudoku
  {
    void DisplayOptionsMenu()
    {
      unsigned int option = 100;
      unsigned int exit = 0;
      std::cout << "\033[1;32mWubin Games - Challenge C++! \033[0m" << std::endl << std::endl;
      while(true)
      {
        std::cout << "\033[1;32mSudoku Type Selection: \033[0m" << std::endl;
        std::cout << "\033[1;33m0. Regular Sudoku 9x9 \033[0m" << std::endl;
        std::cout << "\033[1;33m1. Alphabet Sudoku 9x9 \033[0m" << std::endl;
        std::cout << "\033[1;33m2. Extended Sudoku 16x16 \033[0m" << std::endl;
        std::cout << "\033[1;33m3. Exit \033[0m" << std::endl;

        while(option > 3)
        {
          std::cout << "\033[1;32mPlease Select A Valid Option: \033[0m" << std::endl;
          std::cin >> option;
        }
        switch (option)
        {
          case 0:
            PlaySudokuGame<SudokuBoard>("Regular Sudoku");
            break;
          case 1:
            PlaySudokuGame<AlphaSudokuBoard>("Alphabet Sudoku");
            break;
          case 2:
            PlaySudokuGame<ExtendedSudokuBoard>("ExtendedSudoku");
            break;
          case 3:
          default:
            std::exit(0);
        }
        option = 100;
      }
    }
  }
}