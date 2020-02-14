#pragma once

#include <vector>
#include <list>
#include <stack>
#include <cmath>
#include <assert.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <algorithm>
#include <thread>

#include "Generic/Position.h"

/* SudokuEngine is a set of template functions to implement algorithems of
   every type of Sudoku Game. Each type of Sudoku game should only be diffientiated
   by its value type and width. The rules to play them should be same...
*/

namespace wubinboardgames
{
  namespace sudoku
  {
    // VacantMap sets up a table between each cell and the next vacant cell after it
    // For example, if board[0][3] is the first vacant cell, the table should be like
    //
    // board[0][0], board[0][3]
    // board[0][1], board[0][3]
    // board[0][2], board[0][3]
    //
    typedef std::unordered_map<Position, Position, PositionHashser> VacantMap;

    // Difficulty levels are defined by how many retries are made for the solution.
    enum LEVEL {
      NO_SOLUTION = 0,
      NO_UNIQUE_SOLUTION = 1,
      EASY = 3000,
      MEDIUM = 30000,
      HARD = 50000,
      SAMURAI = 500000,
      EXTREME = 1000000
    };

    /*
    Check if a board has a valid state. A valid state means there is no violation to
    the rule of sudoku. if check_vacant is true, boards containing
    vacant cells will be regarded as invalid.
    */
    template<typename SudokuBoard>
    bool IsBoardValid(const SudokuBoard & board, bool check_vacant = false)
    {
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;
      typedef typename SudokuBoard::Cell::ValueType ValueType;

      const unsigned int cubic_width = std::sqrt(width);
      /* lookup table to determine if the value has already existed
         For example
         if rowFlags[1][3] is true, it means 3 has already existed in row 1
         if colFlags[3][7] is true, it means 3 has already existed in col 7
         if cubicFlags[4][3] is true, it means 3 has already existed in 4th grid.
      */
      bool rowFlags[width][width] = {{false}};
      bool colFlags[width][width] = {{false}};
      bool cubicFlags[width][width] = {{false}};


      unsigned int row = 0, col = 0;
      for(unsigned int index = 0; index < end_index; ++index)
      {
        row = index / width;
        col = index % width;
        if(check_vacant && (!board[row][col].isValid() || board[row][col].isVacant()))
          return false;

        // if the cell is vacant, skip
        if(board[row][col].isValid() && !board[row][col].isVacant())
        {
          // unfortunately, the ValueType should be able to converted to unsigned int...
          unsigned int value = static_cast<unsigned int>(static_cast<ValueType>(board[row][col]));

          // value may be larger than width. Imagine we are playing 9x9 sudoku
          // number of which starts from 10...
          unsigned int value_offset = value % width;
          if(rowFlags[row][value_offset] || colFlags[value_offset][col] ||
             cubicFlags[cubic_width*(row/cubic_width) + col/cubic_width][value_offset])
          {
            return false;
          }
          // if the cell has not existed yet, set flags.
          rowFlags[row][value_offset] = true;
          colFlags[value_offset][col] = true;
          cubicFlags[cubic_width*(row/cubic_width) + col/cubic_width][value_offset] = true;
        }
        // if the cell is not valid and not vacant, the board then is not valid.
        else if(!board[row][col].isValid())
        {
          return false;
        }
      }
      return true;
    }

    // check if the board is finally completed
    template<typename SudokuBoard>
    bool IsBoardSolved(const SudokuBoard & board)
    {
      return IsBoardValid<SudokuBoard>(board, true);
    }

    // Find the first vacant cell in this sudoku board.
    template<typename SudokuBoard>
    inline Position FindVacantPosition(const SudokuBoard & board)
    {
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;
      unsigned int row = 0, col = 0;
      for(unsigned int index = 0; index < end_index; ++index)
      {
        row = index / width;
        col = index % width;
        if(board[row][col].isVacant())
          return Position{row, col};
      }
      return Position{width + 1, width + 1};
    }

    template<typename SudokuBoard>
    inline bool IsPositionValid(const Position & pos)
    {
      constexpr unsigned int width = SudokuBoard::width;
      return (pos.first < width && pos.second < width);
    }

    template<typename SudokuBoard>
    inline bool IsPositionValid(const unsigned int & row, const unsigned int & col)
    {
      constexpr unsigned int width = SudokuBoard::width;
      return (row < width && col < width);
    }

    /* Get an vacant map for the board.
       VacantMap is a table between each cell and the next vacant cell after it
       For example, if board[0][3] is the first vacant cell, the table should be like
        board[0][0], board[0][3]
        board[0][1], board[0][3]
        board[0][2], board[0][3]
    */
    template<typename SudokuBoard>
    VacantMap GetVacantMap(const SudokuBoard & board)
    {
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;
      VacantMap vacant_map;
      unsigned int pre = 0, post = 0;
      // pre proceeds to check if the cell is vacant
      // if it is, pos moves to pre and maps each cell passed with the
      // cell the pre is sitting
      for(; pre < end_index; ++pre)
      {
        unsigned int row = pre / width;
        unsigned int col = pre % width;
        if(board[row][col].isVacant() && pre)
        {
          while(post < pre)
          {
            vacant_map[Position{post/width, post%width}] = Position{row, col};
            ++post;
          }
        }
      }
      // for those left over, map them to an invalid position.
      // width + 1 is out of the board.
      while(post < pre)
      {
        vacant_map[Position{post/width, post%width}] = Position{width + 1, width + 1};
        ++post;
      }
      return vacant_map;
    }

    /* Check if a cell is suitable to be put on the board.
       if it does not violate the rule(no repeating element for each row, col and grid) of sudoku,
       it is elegible.
    */
    template<typename SudokuBoard>
    inline bool IsCellEligible(const SudokuBoard & board, const typename SudokuBoard::Cell & cell)
    {
      if(!cell.isValid())
        return false;
      typedef typename SudokuBoard::Cell::ValueType ValueType;
      constexpr const int width = SudokuBoard::width;
      const unsigned int & grid_width = std::sqrt(width);

      if(!IsPositionValid<SudokuBoard>(cell.getPosition()))
        return false;

      unsigned int row_index, col_index;
      CoordinateConvert(cell.getPosition(), row_index, col_index);

      const ValueType value = static_cast<ValueType>(cell);

      for(unsigned int x = 0; x < width; ++x)
      {
        // check row and col at same time
        if(board[row_index][x] == value || board[x][col_index] == value) 
          return false;
      }
      for(unsigned int row = (row_index/grid_width) * grid_width; row < ((row_index/grid_width + 1) * grid_width); ++row)
      {
        for(unsigned int col = (col_index/grid_width) * grid_width; col < ((col_index/grid_width + 1) * grid_width); ++col)
        {
          if(board[row][col] == value)
            return false;
        }
      }
      return true;
    }

    /*
      Search solutions for a given board. num_of_retries can return the numbers of retries made in the
      process of solving.
    */
    template<typename SudokuBoard>
    std::vector<SudokuBoard> SearchSolution(const SudokuBoard & board, unsigned int * num_of_retries = nullptr)
    {
      constexpr const int width = SudokuBoard::width;
      typedef typename SudokuBoard::Cell Cell;

      std::vector<SudokuBoard> solutions;
      if(!IsBoardValid<SudokuBoard>(board))
        return solutions;

      // we are lucky here. If there is not vacant cell and it is a valid board,
      // return the board...
      if(!IsPositionValid<SudokuBoard>(FindVacantPosition<SudokuBoard>(board)))
      {
        solutions.push_back(board);
        return solutions;
      }

      unsigned int first_vacant_row, first_vacant_col;
      unsigned int num_of_solutions = 0;
      unsigned int num_of_forwards = 0;
      SudokuBoard work_board{board};
      VacantMap board_vacants_map{GetVacantMap(work_board)};

      /*
        In fact, in a sudoku game, we only care about the values of those
        initially vacant cell. We now have the vacant map. And I am going to
        use a stack to keep track on each cell that has been assigned.
        If the number of cells in stack equals to the number of vacants in the
        board, that means our work is done. Otherwise, if there is no solution,
        the stack will finally be empty as each cell has been tried with each possible value.
      */
      std::stack<Cell> stack_of_vacant_cells;

      CoordinateConvert(FindVacantPosition<SudokuBoard>(board), first_vacant_row, first_vacant_col);
      // push the first vacant cell before we get into the loop
      stack_of_vacant_cells.push(board[first_vacant_row][first_vacant_col]);

      unsigned int top_cell_row = width, top_cell_col = width;

      // empty stack means we cannot find a solution with the board.
      while(!stack_of_vacant_cells.empty())
      {
        // either the top one is vacant(never tried before), or it has a valid value(tried previously),
        // can we continue the search.
        while(!stack_of_vacant_cells.empty() &&  (stack_of_vacant_cells.top().isValid() ||
              stack_of_vacant_cells.top().isVacant()))
        {
          // cell_on_top is a copy.
          auto cell_on_top = stack_of_vacant_cells.top();

          // ++cell_on_top means to check if its next value is valid and eligible for the board.
          // this because it is either a vacant cell that has not be tried with or it was tried but failed for
          // next fillable cells. Exit the while loop only if there is not eligible value or it has reached to a valid value.
          while((cell_on_top.isValid() || cell_on_top.isVacant()) && !IsCellEligible<SudokuBoard>(work_board, ++cell_on_top));

          CoordinateConvert(cell_on_top.getPosition(), top_cell_row, top_cell_col);

          // if the cell_on_top is eligible, move to next vacant cell past to it.
          if(cell_on_top.isValid())
          {
            // increment number of retries
            ++num_of_forwards;
            // set the cell on the board.
            work_board[top_cell_row][top_cell_col] = cell_on_top;
            // find next fillable cell
            Position next_vacant_pos = board_vacants_map.at(cell_on_top.getPosition());
            // move cell_on_top as it is approaching to the end of its life.
            stack_of_vacant_cells.top() = std::move(cell_on_top);

            // Bingo! No next vacant cell means we have found a solution!
            if(!IsPositionValid<SudokuBoard>(next_vacant_pos))
              break;
            
            // otherwise push next fillable cell into the stack
            CoordinateConvert(next_vacant_pos, top_cell_row, top_cell_col);
            stack_of_vacant_cells.push(work_board[top_cell_row][top_cell_col]);
          }
          else
          {
            // no eligible value found. pop it.
            stack_of_vacant_cells.pop();
            // reset the top cell on the board to make sure later we can try with
            // it from the minimum value.
            work_board[top_cell_row][top_cell_col].reset();
          }
        }
        // If the stack if not empty, it comes from the break above
        // We find one solution.
        if(!stack_of_vacant_cells.empty())
        {
          // save the solution as there might be more than one.
          solutions.push_back(work_board);
          // break if this is the second solution. No need to find every solution
          // for an invalid game setup.
          if(++num_of_solutions == 2)
            break;
          // if this is the first solution, take the num_of_retries.
          if(num_of_retries)
            *num_of_retries = num_of_forwards;
          // we want to find another solution, so pop the last fillabe cell in
          // the stack. Now the top one was the second last and will do ++ to
          // find another solution.
          stack_of_vacant_cells.pop();
          // reset the last one in the cell.
          work_board[top_cell_row][top_cell_col].reset();
          num_of_forwards = 0;
        }
      }
      return solutions;
    }

    // GetOneSolution no matter it is unique or not.
    // set isUnique to indicate.
    template<typename SudokuBoard>
    SudokuBoard GetOneSolution(const SudokuBoard & board, bool * isUnique = nullptr)
    {
      std::vector<SudokuBoard> solutions = SearchSolution<SudokuBoard>(board);
      if(isUnique)
      {
        *isUnique = (1 == solutions.size()) ? true : false;
      }
      return (0 < solutions.size()) ? solutions[0] : board;
    }

    template<typename SudokuBoard>
    bool IsSolutionUnique(const SudokuBoard & board)
    {
      std::vector<SudokuBoard> solutions = SearchSolution<SudokuBoard>(board);
      return (1 == solutions.size());
    }

    template<typename SudokuBoard>
    bool IsSolvable(const SudokuBoard & board)
    {
      std::vector<SudokuBoard> solutions = SearchSolution<SudokuBoard>(board);
      return (1 <= solutions.size());
    }

    // level evaluation determined by the number of retries.
    template<typename SudokuBoard>
    LEVEL LevelEvaluate(const SudokuBoard & board)
    {
      unsigned int num_of_retries = 0;
      std::vector<SudokuBoard> solutions = SearchSolution<SudokuBoard>(board, &num_of_retries);

      if(0 == solutions.size())
        return LEVEL::NO_SOLUTION;

      if(2 == solutions.size())
        return LEVEL::NO_UNIQUE_SOLUTION;

      if(num_of_retries < LEVEL::EASY)
        return LEVEL::EASY;

      if(num_of_retries < LEVEL::MEDIUM)
        return LEVEL::MEDIUM;

      if(num_of_retries < LEVEL::HARD)
        return LEVEL::HARD;

      if(num_of_retries < LEVEL::SAMURAI)
        return LEVEL::SAMURAI;

      return LEVEL::EXTREME;
    }

    /* Algorithm to generate a final board, that is board complying to rule
       of sudoku and has no vacant cell.
       The algorithm first sets up a table for each cell. The table maps the index
       of the cell with a list of values that can be set to the cell. 
       For sudoku, the default list of values are from 1 to 9.
       Then each time, we randomly pick up a value from the list for each cell, and try
       it to see if the board is still valid. If it is not, remove it from the list.
       If the list becomes empty,
       1. go back to its previous cell and delete its current value
       2. set the current cell with default list to make sure it is ready for next turn
       of trials.
       Back and forth until we find a valid final table.
    */
    template<typename SudokuBoard>
    SudokuBoard GenerateFinalBoard()
    {
      std::srand(std::time(nullptr));

      SudokuBoard work_board;
      typedef typename SudokuBoard::Cell Cell;
      typedef typename Cell::ValueType ValueType;
      constexpr unsigned int board_width = SudokuBoard::width;
      constexpr unsigned int end_index = board_width * board_width;
      constexpr unsigned int value_width = Cell::values_length;
      constexpr ValueType minimum = Cell::minimum_value;

      std::vector<std::list<ValueType>> cells_candidate_values;
      std::list<ValueType> cell_default_values;

      for(unsigned int i = 0; i < value_width; ++i)
      {
        cell_default_values.push_back(minimum + i);
      }
      for(unsigned int i = 0; i < board_width * board_width; ++i)
      {
        // set for each cell
        cells_candidate_values.push_back(cell_default_values);
      }
      unsigned int pos = 0;
      while(!IsBoardSolved<SudokuBoard>(work_board) && pos < end_index)
      {
        ValueType value;
        unsigned int steps_forward = 0;
        while(0 < cells_candidate_values[pos].size())
        {
          // pick up a random value from the cell's candidate list
          steps_forward = std::rand() % cells_candidate_values[pos].size();
          auto itr_list = cells_candidate_values[pos].begin();
          std::advance (itr_list,steps_forward);
          value  = *itr_list;
          work_board[pos/board_width][pos%board_width] = value;

          // Bingo! we find a suitable final board.
          if(IsBoardValid(work_board))
              break;
          // otherwise, remove the value and try another random value from the list.
          auto it = std::find(cells_candidate_values[pos].begin(), cells_candidate_values[pos].end(), value);
          cells_candidate_values[pos].erase(it);
          work_board[pos/board_width][pos%board_width].reset();
        }
        // no suitable value found for this cell, set it default and go back
        // to its previous cell.
        if(0 == cells_candidate_values[pos].size())
        {
          work_board[pos/board_width][pos%board_width].reset();
          cells_candidate_values[pos] = cell_default_values;
          auto value_of_last_cell = static_cast<ValueType>(work_board[(pos-1) / board_width][(pos-1) % board_width]);
          auto it = std::find(cells_candidate_values[pos-1].begin(), cells_candidate_values[pos-1].end(), value_of_last_cell);
          cells_candidate_values[pos - 1].erase(it);
          --pos;
        }
        // if the value is suitable, proceed to next cell.
        else
        {
          ++pos;
        }
      }
      return work_board;
    }

    /*
      GenerateSolvableBoard returns a solvable board with given difficulty level.
      You can aslo determine the minimum number of vacant cells the board must have.
      Befault, it is width*width / 2.5. For sudoku, it is 32.
      The algorithm will first get a final table and then try to set some cells vacant
      to generate a solvable game.
      If setting a cell vacant makes the board have no solution or more than one solution, it
      recover the cell back and tries to set another cell vacant.
      After certain amount of tries, if the solvable board still cannot be generated complying to
      the given level. The algorithm will require a new final board and retry.
    */
    template<typename SudokuBoard>
    SudokuBoard GenerateSolvableBoard(const LEVEL & level = LEVEL::MEDIUM,
                                      const unsigned int & minimum_empties = SudokuBoard::width * SudokuBoard::width / 2.5)
    {
      typedef typename SudokuBoard::Cell Cell;
      typedef typename Cell::ValueType ValueType;
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;

      std::srand(std::time(nullptr));
      if(level < LEVEL::EASY)
      {
        return SudokuBoard{};
      }
      SudokuBoard work_board;

      unsigned int num_of_empties = 0;
      unsigned int num_of_retries = 0;
      unsigned int index = 0;
      work_board = GenerateFinalBoard<SudokuBoard>();
      // I realize it is a good opportunity to testing the IsSolutionUnique function here.
      // As GenerateFinalBoard does not rely on SearchSolution, we can solve the solvable board by
      // SearchSolution and compare it with the final board initially returned by GenerateFinalBoard.
      // If for all time they match, we can assume that IsSolutionUnique works fine.
#ifdef _testing
      SudokuBoard testing_board;
      testing_board = work_board;
      unsigned int num_of_testing = 100;
      while(--num_of_testing){
#endif
      while(true)
      {
        // randomly pick up a number, if it is already vacant, skip
        index = std::rand() % end_index;
        if(work_board[index/width][index%width].isVacant())
          continue;

        // keep its value and set it to vacant.
        ValueType value = static_cast<ValueType>(work_board[index/width][index%width]);
        work_board[index/width][index%width].reset();

        if(IsSolutionUnique<SudokuBoard>(work_board))
        {
          ++num_of_empties;
          num_of_retries = 0;
          //Bingo! We find the solvable board with given level.
          if(level == LevelEvaluate<SudokuBoard>(work_board) && num_of_empties > minimum_empties)
            break;
        }
        else
        {
          ++num_of_retries;
          work_board[index/width][index%width] = value;
        }
        // try certain amount of times. For soduko, it tries 121.
        // if no solvable board found, ask for a new final board.
        if(num_of_retries > (end_index * 1.5))
        {
          work_board = GenerateFinalBoard<SudokuBoard>();
          num_of_empties = num_of_retries = 0;
#ifdef _testing
          testing_board = work_board;
#endif
        }
      }
#ifdef _testing
      std::cout << "Testing: " << num_of_testing << " th"<< std::endl;
      assert(testing_board == SearchSolution<SudokuBoard>(work_board)[0]);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
#endif
      return work_board;
  }

  /* 
    Made to play with IsSolutionUnique
  */
  template<typename SudokuBoard>
  SudokuBoard GenerateRandomBoard()
  {
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;

      std::srand(std::time(nullptr));
      unsigned int num_of_empty_cells = std::rand() % end_index;
      SudokuBoard work_board = GenerateFinalBoard<SudokuBoard>();
      while(--num_of_empty_cells)
      {
        unsigned int index = std::rand() % end_index;
        work_board[index/width][index%width].reset();
      }
      return work_board;
  }
}
}