#pragma once

#include <string>
#include <array>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <sstream>
#include <memory>
#include <functional>

#include "Position.h"

namespace wubinboardgames
{
  constexpr const char * DEFAULT_FILE_NAME = "Board.data";

  /*GenericBoard is a template class that can be instantiated to hold any
    type of cells and width. The number of cells is specialized by WIDTH * WIDTH
  */
  template<typename CELL, unsigned int WIDTH = CELL::values_length>
  struct  GenericBoard
  {
    static constexpr unsigned int width = WIDTH;
    typedef CELL Cell;
    typedef std::array<CELL, width> Row;
    typedef std::array<Row, width> Board;
    typedef std::array<std::array<typename CELL::ValueType, width>, width> RawValueBoard;

    GenericBoard();
    // rule of five
    virtual ~GenericBoard() = default;
    GenericBoard(const GenericBoard & another) = default;
    GenericBoard(GenericBoard && another) = default;
    GenericBoard & operator=(const GenericBoard & another) = default;
    GenericBoard & operator=(GenericBoard && another) = default;

    // Conversion and comparison are enabled between GenericBoard and raw board
    GenericBoard(const RawValueBoard & rawValueBoard);
    explicit GenericBoard(const Board & anotherBoard);
    explicit GenericBoard(Board && anotherBoard);
    GenericBoard & operator=(const Board & another);
    GenericBoard & operator=(Board && another);
    GenericBoard & operator=(const RawValueBoard & rawValueBoard);
    bool operator==(const GenericBoard & another) const;
    bool operator==(const Board & anotherBoard) const;
    bool operator==(const RawValueBoard & rawValueBoard) const;

    bool writeToFile(const std::string & path = DEFAULT_FILE_NAME) const;
    bool loadFromFile(const std::string & path);

    void clear();
    void backup();
    void reset();

    // I admit it is not a good practice to expose the underling row...
    // But to make the board easy to be accessed.
    Row & operator[](const unsigned int & row_num);

    // called by const Board & 
    const Row & operator[](const unsigned int & row_num) const;

    friend std::ostream & operator<<(std::ostream & os, const GenericBoard & genericBoard)
    {
      // content is used to remove the trailling spaces
      // not a good practice, but it works for now.
      std::string content;
      const unsigned int cubic_width = std::sqrt(GenericBoard::width);
      for(unsigned int row = 0; row < width; ++row)
      {
        // stringstream to remove the trailling spaces
        // not a good practice, but it works for now.
        std::stringstream row_str;
        for(unsigned int col = 0; col < width; ++col)
        {
          if(genericBoard.board[row][col].isVacant())
          {
            // width 2 for 16*16 board
            row_str.width(2);
            row_str << '0' << " ";
          }
          else
          {
            row_str.width(2);
            row_str << genericBoard.board[row][col] << " ";
          }
          // for 9*9 sudoku, add a space between each 3 columns
          if ((col + 1) % cubic_width == 0)
            row_str << " ";
        }
        content = row_str.str();
        content.replace(content.size()-2, 2, "");
        os << content;
        os << "\n";
        if((row + 1) % cubic_width == 0)
          os << "\n";
      }
      return os;
    }

    friend std::istream & operator>>(std::istream & is, GenericBoard & genericBoard)
    {
      constexpr unsigned int end_index = width * width;
      unsigned int row = 0, col = 0;
      for(unsigned int index = 0; index < end_index; ++index)
      {
        row = index / width;
        col = index % width;
        // this requires the CELL must overload its own input operator
        is >> genericBoard.board[row][col];
      }
      return is;
    }

    private:
    Board board;
    Board backup_board;
  };

  template<typename CELL, unsigned int WIDTH>
  GenericBoard<CELL, WIDTH>::GenericBoard()
  {
    // check in compile-time. do not allow width > 17 or width < 4
    static_assert(width < 17, "Width of the board cannot be larger than 16.");
    static_assert(width > 3, "Width of the board cannot be less than 4");
    constexpr unsigned int end_index = width * width;
    unsigned int row = 0, col = 0;
    for(unsigned int index = 0; index < end_index; ++index)
    {
      row = index / width;
      col = index % width;
      // set position for each cell
      board[row][col].setPosition(row, col);
      board[row][col].reset();
    }
  }

  template<typename CELL, unsigned int WIDTH>
  GenericBoard<CELL, WIDTH>::GenericBoard(const Board & anotherBoard) : board(anotherBoard)
  {}

  template<typename CELL, unsigned int WIDTH>
  GenericBoard<CELL,WIDTH>::GenericBoard(Board && anotherBoard) : board(anotherBoard)
  {}

  template<typename CELL, unsigned int WIDTH>
  GenericBoard<CELL,WIDTH>::GenericBoard(const RawValueBoard & rawValueBoard)
  {
    constexpr unsigned int end_index = width * width;
    for(unsigned int index = 0; index < end_index; ++index)
    {
      // position should already be set by default constructor.. only to be safe
      board[index/width][index%width].setPosition(index/width, index%width);
      // set value for each cell. This requires cell must overload operator=(ValueType).
      board[index/width][index%width] = rawValueBoard[index/width][index%width];
    }
  }

  template<typename CELL, unsigned int WIDTH>
  GenericBoard<CELL,WIDTH> & GenericBoard<CELL,WIDTH>::operator=(const Board & anotherBoard)
  {
    board = anotherBoard;
  }

  template<typename CELL, unsigned int WIDTH>
  GenericBoard<CELL,WIDTH> & GenericBoard<CELL,WIDTH>::operator=(Board && anotherBoard)
  {
    board = anotherBoard;
  }

  template<typename CELL, unsigned int WIDTH>
  GenericBoard<CELL,WIDTH> & GenericBoard<CELL,WIDTH>::operator=(const GenericBoard::RawValueBoard & rawValueBoard)
  {
    constexpr unsigned int end_index = width * width;
    for(unsigned int index = 0; index < end_index; ++index)
    {
      board[index/width][index%width] = rawValueBoard[index/width][index%width];
    }
  }

  template<typename CELL, unsigned int WIDTH>
  bool GenericBoard<CELL,WIDTH>::operator==(const RawValueBoard & rawValueBoard) const
  {
    constexpr unsigned int end_index = width * width;
    for(unsigned int index = 0; index < end_index; ++index)
    {
      if(!(board[index/width][index%width] == rawValueBoard[index/width][index%width]))
        return false;
    }
    return true;
  }

  template<typename CELL, unsigned int WIDTH>
  bool GenericBoard<CELL,WIDTH>:: operator==(const Board & anotherBoard) const
  {
    constexpr unsigned int end_index = width * width;
    for(unsigned int index = 0; index < end_index; ++index)
    {
      if(!(board[index/width][index%width] == anotherBoard[index/width][index%width]))
        return false;
    }
    return true;

  }

  template<typename CELL, unsigned int WIDTH>
  bool GenericBoard<CELL,WIDTH>::operator==(const GenericBoard<CELL,WIDTH> & another) const
  {
    return (this->board == another.board);
  }

  template<typename CELL, unsigned int WIDTH>
  void GenericBoard<CELL,WIDTH>::clear()
  {
    constexpr unsigned int end_index = width * width;
    for(unsigned int index = 0; index < end_index; ++index)
    {
      board[index/width][index%width].reset();
    }
  }

  template<typename CELL, unsigned int WIDTH>
  void GenericBoard<CELL,WIDTH>::backup()
  {
    backup_board = board;
  }

  template<typename CELL, unsigned int WIDTH>
  void GenericBoard<CELL,WIDTH>::reset()
  {
    board = backup_board;
  }

  template<typename CELL, unsigned int WIDTH>
  typename GenericBoard<CELL,WIDTH>::Row &
      GenericBoard<CELL,WIDTH>::operator[](const unsigned int & row_num)
  {
    return board[row_num];
  }

  template<typename CELL, unsigned int WIDTH>
  const typename GenericBoard<CELL,WIDTH>::Row &
      GenericBoard<CELL,WIDTH>::operator[](const unsigned int & row_num) const
  {
    return board[row_num];
  }

  template<typename CELL, unsigned int WIDTH>
  bool GenericBoard<CELL,WIDTH>::writeToFile(const std::string & path) const
  {
    std::ofstream ofs(path, std::ofstream::out);
    if(ofs)
    {
      // call operator<<
      ofs << *this;
      ofs.close();
      return true;
    }
    ofs.close();
    return false;
  }

  template<typename CELL, unsigned int WIDTH>
  bool GenericBoard<CELL,WIDTH>::loadFromFile(const std::string & path)
  {
    std::ifstream ifs(path);
    if(ifs)
    {
      // call operator>>
      ifs >> *this;
      ifs.close();
      return true;
    }
    ifs.close();
    return false;
  }
}