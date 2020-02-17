#include "gtest/gtest.h"
#include "Sudoku/SudokuEngine.h"
#include "Sudoku/SudokuBoard.h"
#include "Sudoku/SudokuGame.h"

namespace wubinboardgames
{
  namespace sudoku
  {
    TEST(SudokuEngineUnitTesting, validate)
    {
      SudokuBoard sudoku_board;
      EXPECT_TRUE(IsBoardValid<SudokuBoard>(sudoku_board));
      sudoku_board[0][0] = 3;
      sudoku_board[0][8] = 3;
      EXPECT_FALSE(IsBoardValid<SudokuBoard>(sudoku_board));
      ++sudoku_board[0][8];
      EXPECT_TRUE(IsBoardValid<SudokuBoard>(sudoku_board));
      sudoku_board[7][0] = 3;
      EXPECT_FALSE(IsBoardValid<SudokuBoard>(sudoku_board));
    }
    TEST(SudokuEngineUnitTesting, solved)
    {
      SudokuBoard sudoku_board;
      EXPECT_FALSE(IsBoardSolved<SudokuBoard>(sudoku_board));
      sudoku_board.loadFromFile("solved.board");
      EXPECT_TRUE(IsBoardSolved<SudokuBoard>(sudoku_board));
    }
    TEST(SudokuEngineUnitTesting, findvacantposition)
    {
      SudokuBoard sudoku_board;
      sudoku_board[0][0] = 1;
      sudoku_board[0][1] = 1;
      sudoku_board[0][2] = 1;
      sudoku_board[0][3] = 1;
      sudoku_board[0][4] = 1;
      Position pos = FindVacantPosition<SudokuBoard>(sudoku_board);
      unsigned int row, col;
      CoordinateConvert(pos, row, col);
      EXPECT_EQ(5, col);
      EXPECT_EQ(0, row);
    }
    TEST(SudokuEngineUnitTesting, ispositionvalid)
    {
      EXPECT_FALSE(IsPositionValid<SudokuBoard>(9, 9));
      EXPECT_TRUE(IsPositionValid<SudokuBoard>(4, 5));
    }
    TEST(SudokuEngineUnitTesting, vacantmap)
    {
      SudokuBoard sudoku_board;
      sudoku_board.loadFromFile("unsolved.board");
      VacantMap vacant_map{GetVacantMap<SudokuBoard>(sudoku_board)};
      Position p1{1, 0};
      Position pos = vacant_map[p1];
      EXPECT_EQ(pos, Position(1,1));

      p1 = {3, 4};
      pos = vacant_map[p1];
      EXPECT_EQ(pos, Position(3,6));

      p1 = {8, 8};
      pos = vacant_map[p1];
      EXPECT_EQ(pos, Position(10,10));
    }
    TEST(SudokuEngineUnitTesting, iscelleligible)
    {
      SudokuBoard sudoku_board;
      sudoku_board.loadFromFile("unsolved.board");
      SudokuCell cell;
      cell.setPosition(4, 5);
      cell = 3;
      EXPECT_FALSE(IsCellEligible<SudokuBoard>(sudoku_board, cell));
      cell = 4;
      EXPECT_FALSE(IsCellEligible<SudokuBoard>(sudoku_board, cell));
      cell = 7;
      EXPECT_TRUE(IsCellEligible<SudokuBoard>(sudoku_board, cell));
    }
    TEST(SudokuEngineUnitTesting, searchsolution)
    {
      SudokuBoard sudoku_board;
      sudoku_board.loadFromFile("unsolved.board");
      SudokuBoard sovled_board;
      sovled_board.loadFromFile("solved.board");
      std::vector<SudokuBoard> solutions{SearchSolution<SudokuBoard>(sudoku_board)};
      EXPECT_EQ(solutions[0], sovled_board);
    }
    TEST(SudokuEngineUnitTesting, issolutionunique)
    {
      SudokuBoard sudoku_board;
      sudoku_board.loadFromFile("unsolved.board");
      EXPECT_TRUE(IsSolutionUnique<SudokuBoard>(sudoku_board));
      sudoku_board.loadFromFile("unsolvable.board");
      EXPECT_FALSE(IsSolutionUnique<SudokuBoard>(sudoku_board));
    }
    TEST(SudokuEngineUnitTesting, issolvable)
    {
      SudokuBoard sudoku_board;
      sudoku_board.loadFromFile("unsolvable.board");
      EXPECT_TRUE(IsSolvable<SudokuBoard>(sudoku_board));
    }
    TEST(SudokuEngineUnitTesting, levelevaluate)
    {
      SudokuBoard sudoku_board;
      sudoku_board.loadFromFile("unsolved.board");
      EXPECT_EQ(LevelEvaluate<SudokuBoard>(sudoku_board), LEVEL::SAMURAI);
    }
    TEST(SudokuEngineUnitTesting, generatefinalboard)
    {
      unsigned int i = 20;
      while(--i)
      {
        SudokuBoard sudoku_board{GenerateFinalBoard<SudokuBoard>()};
        EXPECT_TRUE(IsBoardSolved<SudokuBoard>(sudoku_board));
      }
    }
    TEST(SudokuEngineUnitTesting, generatesolvableboard)
    {
      unsigned int i = 10;
      while(--i)
      {
        SudokuBoard sudoku_board{GenerateSolvableBoard<SudokuBoard>()};
        EXPECT_TRUE(IsSolutionUnique<SudokuBoard>(sudoku_board));
      }
    }
    TEST(SudokuEngineUnitTesting, generateeasyboard)
    {
        SudokuBoard sudoku_board{GenerateSolvableBoard<SudokuBoard>(LEVEL::EASY)};
        EXPECT_TRUE(IsSolutionUnique<SudokuBoard>(sudoku_board));
        EXPECT_EQ(LevelEvaluate<SudokuBoard>(sudoku_board), LEVEL::EASY);
    }
    TEST(SudokuEngineUnitTesting, generatemediumboard)
    {
        SudokuBoard sudoku_board{GenerateSolvableBoard<SudokuBoard>(LEVEL::MEDIUM)};
        EXPECT_TRUE(IsSolutionUnique<SudokuBoard>(sudoku_board));
        EXPECT_EQ(LevelEvaluate<SudokuBoard>(sudoku_board), LEVEL::MEDIUM);
    }
  }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}