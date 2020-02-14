#pragma once

#include "Generic/GenericCell.h"

namespace wubinboardgames
{
  namespace sudoku
  {
    // instantiated to each type of cells.
    typedef GenericCell<unsigned int, 1, 9> SudokuCell;
    typedef GenericCell<uint8_t, 'a', 9> AlphaSudokuCell;
    typedef GenericCell<uint8_t, ' ', 9> PunctuationSudokuCell;
    typedef GenericCell<unsigned int, 1, 16> ExtendedSudokuCell;
    typedef GenericCell<uint8_t, 'a', 16> ExtendedAlphaSudokuCell;
  }
}