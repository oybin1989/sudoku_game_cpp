#pragma once

#include "Generic/GenericBoard.h"
#include "SudokuCell.h"

namespace wubinboardgames
{
  namespace sudoku
  {
    // instantiated to each type of boards.
    typedef GenericBoard<SudokuCell> SudokuBoard;
    typedef GenericBoard<AlphaSudokuCell> AlphaSudokuBoard;
    typedef GenericBoard<ExtendedSudokuCell> ExtendedSudokuBoard;
    typedef GenericBoard<PunctuationSudokuCell> PunctuationSudokuBoard;
    typedef GenericBoard<ExtendedAlphaSudokuCell> ExtendedAlphaSudokuBoard;
  }

}