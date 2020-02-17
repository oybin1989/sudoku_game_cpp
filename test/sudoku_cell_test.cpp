#include "gtest/gtest.h"
#include "Sudoku/SudokuCell.h"


namespace wubinboardgames
{
  namespace sudoku
  {
    TEST(SudokuCellUnitTesting, initial)
    {
      SudokuCell sudoku_cell;
      ASSERT_TRUE(sudoku_cell.isVacant());
      ASSERT_TRUE(sudoku_cell.isValid());
    }
    TEST(SudokuCellUnitTesting, increment)
    {
      SudokuCell sudoku_cell;
      ++sudoku_cell;
      ASSERT_FALSE(sudoku_cell.isVacant());
      ASSERT_TRUE(sudoku_cell.isValid());
    }
    TEST(SudokuCellUnitTesting, conversion)
    {
      SudokuCell sudoku_cell;
      ++sudoku_cell;
      ASSERT_EQ(1, static_cast<SudokuCell::ValueType>(sudoku_cell));
    }
    TEST(SudokuCellUnitTesting, addition_assignment)
    {
      SudokuCell sudoku_cell;
      sudoku_cell += 3;
      ASSERT_FALSE(sudoku_cell.isVacant());
      ASSERT_TRUE(sudoku_cell.isValid());
      ASSERT_EQ(3, static_cast<SudokuCell::ValueType>(sudoku_cell));
    }
    TEST(SudokuCellUnitTesting, assignment)
    {
      SudokuCell sudoku_cell;
      sudoku_cell = 4;
      ASSERT_FALSE(sudoku_cell.isVacant());
      ASSERT_TRUE(sudoku_cell.isValid());
      ASSERT_EQ(4, static_cast<SudokuCell::ValueType>(sudoku_cell));
    }
    TEST(SudokuCellUnitTesting, copy_assignment)
    {
      SudokuCell sudoku_cell;
      sudoku_cell = 4;
      SudokuCell another_cell;
      another_cell = sudoku_cell;
      ASSERT_EQ(another_cell, sudoku_cell);
    }
    TEST(SudokuCellUnitTesting, move_assignment)
    {
      SudokuCell sudoku_cell;
      sudoku_cell = 4;
      SudokuCell another_cell;
      another_cell = std::move(sudoku_cell);
      ASSERT_EQ(4, static_cast<SudokuCell::ValueType>(another_cell));
    }
    TEST(SudokuCellUnitTesting, resest)
    {
      SudokuCell sudoku_cell;
      sudoku_cell = 4;
      ASSERT_FALSE(sudoku_cell.isVacant());
      sudoku_cell.reset();
      ASSERT_TRUE(sudoku_cell.isVacant());
    }
    TEST(SudokuCellUnitTesting, range)
    {
      SudokuCell sudoku_cell;
      sudoku_cell += 10;
      ASSERT_FALSE(sudoku_cell.isVacant());
      ASSERT_FALSE(sudoku_cell.isValid());
    }
    TEST(SudokuCellUnitTesting, set_get_position)
    {
      SudokuCell sudoku_cell;
      sudoku_cell.setPosition(3, 5);
      unsigned int row, col;
      sudoku_cell.getPosition(row, col);
      ASSERT_EQ(row , 3);
      ASSERT_EQ(col , 5);
    }
  }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}