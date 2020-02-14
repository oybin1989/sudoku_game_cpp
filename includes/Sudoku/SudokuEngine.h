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

namespace wubinboardgames
{
  namespace sudoku
  {
    typedef std::unordered_map<Position, Position, PositionHashser> VacantMap;
    enum LEVEL {
      NO_SOLUTION = 0,
      NO_UNIQUE_SOLUTION = 1,
      EASY = 3000,
      MEDIUM = 30000,
      HARD = 45000,
      SAMURAI = 100000
    };

    template<typename SudokuBoard>
    bool IsBoardValid(const SudokuBoard & board, bool check_vacant = false)
    {
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;
      typedef typename SudokuBoard::Cell::ValueType ValueType;

      const unsigned int cubic_width = std::sqrt(width);
      bool rowFlags[width][width] = {{false}};
      bool colFlags[width][width] = {{false}};
      bool cubicFlags[width][width] = {{false}};


      auto row = 0, col = 0;
      for(auto index = 0; index < end_index; ++index)
      {
        row = index / width;
        col = index % width;
        if(check_vacant && (!board[row][col].isValid() || board[row][col].isVacant()))
          return false;

        if(board[row][col].isValid() && !board[row][col].isVacant())
        {
          unsigned int value = static_cast<unsigned int>(static_cast<ValueType>(board[row][col]));
          unsigned int value_offset = value % width;
          if(rowFlags[row][value_offset] || colFlags[value_offset][col] ||
             cubicFlags[cubic_width*(row/cubic_width) + col/cubic_width][value_offset])
          {
            return false;
          }
          rowFlags[row][value_offset] = true;
          colFlags[value_offset][col] = true;
          cubicFlags[cubic_width*(row/cubic_width) + col/cubic_width][value_offset] = true;
        }
        else if(!board[row][col].isValid())
        {
          return false;
        }
      }
      return true;
    }

    template<typename SudokuBoard>
    bool IsBoardSolved(const SudokuBoard & board)
    {
      return IsBoardValid<SudokuBoard>(board, true);
    }

    template<typename SudokuBoard>
    inline Position FindVacantPosition(const SudokuBoard & board)
    {
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;
      auto row = 0, col = 0;
      for(auto index = 0; index < end_index; ++index)
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

    template<typename SudokuBoard>
    VacantMap GetVacantMap(const SudokuBoard & board)
    {
      constexpr unsigned int width = SudokuBoard::width;
      constexpr unsigned int end_index = width * width;
      VacantMap vacant_map;
      unsigned int pre = 0, post = 0;
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
      while(post < pre)
      {
        vacant_map[Position{post/width, post%width}] = Position{width + 1, width + 1};
        ++post;
      }
      return vacant_map;
    }

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

      for(auto x = 0; x < width; ++x)
      {
        if(board[row_index][x] == value || board[x][col_index] == value) 
          return false;
      }
      for(auto row = (row_index/grid_width) * grid_width; row < ((row_index/grid_width + 1) * grid_width); ++row)
      {
        for(auto col = (col_index/grid_width) * grid_width; col < ((col_index/grid_width + 1) * grid_width); ++col)
        {
          if(board[row][col] == value)
            return false;
        }
      }
      return true;
    }

    template<typename SudokuBoard>
    std::vector<SudokuBoard> SearchSolution(const SudokuBoard & board, int * num_of_retries = nullptr)
    {
      constexpr const int width = SudokuBoard::width;
      typedef typename SudokuBoard::Cell Cell;

      std::vector<SudokuBoard> solutions;
      if(!IsBoardValid<SudokuBoard>(board))
        return solutions;
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
      std::stack<Cell> stack_of_vacant_cells;

      CoordinateConvert(FindVacantPosition<SudokuBoard>(board), first_vacant_row, first_vacant_col);
      stack_of_vacant_cells.push(board[first_vacant_row][first_vacant_col]);

      unsigned int top_cell_row = width, top_cell_col = width;
      while(!stack_of_vacant_cells.empty())
      {
        while(!stack_of_vacant_cells.empty() &&  (stack_of_vacant_cells.top().isValid() ||
              stack_of_vacant_cells.top().isVacant()))
        {
          auto cell_on_top = stack_of_vacant_cells.top();
          while((cell_on_top.isValid() || cell_on_top.isVacant()) && !IsCellEligible<SudokuBoard>(work_board, ++cell_on_top));

          CoordinateConvert(cell_on_top.getPosition(), top_cell_row, top_cell_col);
          if(cell_on_top.isValid())
          {
            ++num_of_forwards;
            work_board[top_cell_row][top_cell_col] = cell_on_top;
            Position next_vacant_pos = board_vacants_map.at(cell_on_top.getPosition());
            stack_of_vacant_cells.top() = std::move(cell_on_top);
            if(!IsPositionValid<SudokuBoard>(next_vacant_pos))
              break;
            CoordinateConvert(next_vacant_pos, top_cell_row, top_cell_col);
            stack_of_vacant_cells.push(work_board[top_cell_row][top_cell_col]);
          }
          else
          {
            stack_of_vacant_cells.pop();
            work_board[top_cell_row][top_cell_col].reset();
          }
        }
        if(!stack_of_vacant_cells.empty())
        {
          solutions.push_back(work_board);
          if(++num_of_solutions == 2)
            break;
          if(num_of_retries)
            *num_of_retries = num_of_forwards;
          stack_of_vacant_cells.pop();
          work_board[top_cell_row][top_cell_col].reset();
          num_of_forwards = 0;
        }
      }
      return solutions;
    }

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

    template<typename SudokuBoard>
    LEVEL LevelEvaluate(const SudokuBoard & board)
    {
      int num_of_retries = 0;
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

      return LEVEL::SAMURAI;
    }
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

      for(auto i = 0; i < value_width; ++i)
      {
        cell_default_values.push_back(minimum + i);
      }
      for(auto i = 0; i < board_width * board_width; ++i)
      {
        cells_candidate_values.push_back(cell_default_values);
      }

      unsigned int pos = 0;
      while(!IsBoardSolved<SudokuBoard>(work_board) && pos < end_index)
      {
        ValueType value;
        unsigned int steps_forward = 0;
        while(0 < cells_candidate_values[pos].size())
        {
          steps_forward = std::rand() % cells_candidate_values[pos].size();
          auto itr_list = cells_candidate_values[pos].begin();
          std::advance (itr_list,steps_forward);
          value  = *itr_list;
          work_board[pos/board_width][pos%board_width] = value;
          if(IsBoardValid(work_board))
              break;
          auto it = std::find(cells_candidate_values[pos].begin(), cells_candidate_values[pos].end(), value);
          cells_candidate_values[pos].erase(it);
          work_board[pos/board_width][pos%board_width].reset();
        }
        if(0 == cells_candidate_values[pos].size())
        {
          work_board[pos/board_width][pos%board_width].reset();
          cells_candidate_values[pos] = cell_default_values;
          auto value_of_last_cell = static_cast<ValueType>(work_board[(pos-1) / board_width][(pos-1) % board_width]);
          auto it = std::find(cells_candidate_values[pos-1].begin(), cells_candidate_values[pos-1].end(), value_of_last_cell);
          cells_candidate_values[pos - 1].erase(it);
          --pos;
        }
        else
        {
          ++pos;
        }
      }
      return work_board;
    }
    template<typename SudokuBoard>
    SudokuBoard GenerateSolvableBoard(const LEVEL & level = LEVEL::MEDIUM,
                                      const int & minimum_empties = SudokuBoard::width * SudokuBoard::width / 4)
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
#ifdef _testing
      SudokuBoard testing_board;
      testing_board = work_board;
      unsigned int num_of_testing = 100;
      while(--num_of_testing){
#endif
      while(true)
      {
        index = std::rand() % end_index;
        if(work_board[index/width][index%width].isVacant())
          continue;

        ValueType value = static_cast<ValueType>(work_board[index/width][index%width]);

        work_board[index/width][index%width].reset();

        if(IsSolutionUnique<SudokuBoard>(work_board))
        {
          ++num_of_empties;
          num_of_retries = 0;
          if(level == LevelEvaluate<SudokuBoard>(work_board) && num_of_empties > minimum_empties)
            break;
        }
        else
        {
          ++num_of_retries;
          work_board[index/width][index%width] = value;
        }
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

  template<typename SudokuBoard>
  SudokuBoard GenerateRandomBoard()
  {
      typedef typename SudokuBoard::Cell Cell;
      typedef typename Cell::ValueType ValueType;
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