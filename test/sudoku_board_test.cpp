#include <cstdio> // std::remove

#include "gtest/gtest.h"
#include "Sudoku/SudokuBoard.h"


namespace wubinboardgames
{
  namespace sudoku
  {
    TEST(SudokuBoardUnitTest, initial)
    {
      SudokuBoard board;
      unsigned int wdith = SudokuBoard::width;
      ASSERT_EQ(wdith, 9);
      for(unsigned int row = 0; row < 9 ; ++row)
      {
        for(unsigned int col = 0; col < 9; ++col)
        {
          ASSERT_TRUE(board[row][col].isVacant());
        }
      }
    }
    TEST(SudokuBoardUnitTest, assignment)
    {
      SudokuBoard board;
      ++board[0][0];
      ASSERT_FALSE(board[0][0].isVacant());
      ASSERT_TRUE(board[0][0].isValid());
      board[0][0] = 10;
      ASSERT_FALSE(board[0][0].isValid());
    }
    TEST(SudokuBoardUnitTest, ok)
    {
      SudokuBoard board;
    }
    TEST(SudokuBoardUnitTest, copy_assignment)
    {
      SudokuBoard board;
      for(unsigned int row = 0; row < 9; ++row)
      {
        for(unsigned int col = 0; col < 9; ++col)
        {
          board[row][col] = col;
        }
      }
      SudokuBoard another_board;
      another_board = board;
      ASSERT_EQ(another_board, board);
    }
    TEST(SudokuBoardUnitTest, move_assignment)
    {
      SudokuBoard board;
      for(unsigned int row = 0; row < 9; ++row)
      {
        for(unsigned int col = 0; col < 9; ++col)
        {
          board[row][col] = col;
        }
      }
      SudokuBoard another_board;
      another_board = std::move(board);
      for(unsigned int row = 0; row < 9; ++row)
      {
        for(unsigned int col = 0; col < 9; ++col)
        {
          ASSERT_EQ(another_board[row][col], col);
        }
      }
    }
    TEST(SudokuBoardUnitTest, clear)
    {
      SudokuBoard board;
      for(unsigned int row = 0; row < 9; ++row)
      {
        for(unsigned int col = 0; col < 9; ++col)
        {
          board[row][col] = col;
        }
      }
      board.clear();
      for(unsigned int row = 0; row < 9; ++row)
      {
        for(unsigned int col = 0; col < 9; ++col)
        {
          ASSERT_TRUE(board[row][col].isVacant());
        }
      }
    }
    TEST(SudokuBoardUnitTest, write_and_read)
    {
      SudokuBoard board;
      for(unsigned int row = 0; row < 9; ++row)
      {
        for(unsigned int col = 0; col < 9; ++col)
        {
          board[row][col] = row ^ col;
        }
      }
      board.writeToFile("Gtest_board");
      SudokuBoard another_board;
      another_board.loadFromFile("Gtest_board");
      std::remove("Gtest_board");
      ASSERT_EQ(board, another_board);
    }
  }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}