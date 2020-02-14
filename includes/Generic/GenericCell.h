#pragma once

#include <iostream>

#include "Position.h"

namespace wubinboardgames
{
  /* GenericCell can be filled into a GenericBoard
     It is instantiated by ValueType, minimum value and
     the range of value.
     For sudoku cell, it is <unsigned int, 1, 9>
  */
  template<typename T, T MIN, unsigned int WIDTH>
  struct GenericCell
  {
    static constexpr T minimum_value = MIN;
    // Unfortunately, T should be able to added and minus an
    // integer... T must overload operator+(T) and operator+(unsigned int)...
    static constexpr T maximum_value = MIN + WIDTH - 1;
    static constexpr T values_length = WIDTH;
    typedef T ValueType;

    GenericCell(unsigned int x, unsigned int y, T raw_value=MIN-1);
    // MIN - 1 means the cell is vacant. set vacant by default
    GenericCell(T=MIN-1);

    // Rule of five
    virtual ~GenericCell() = default;
    GenericCell(const GenericCell & another) = default;
    GenericCell(GenericCell && another) = default;
    GenericCell & operator=(GenericCell && another) = default;
    GenericCell & operator=(const GenericCell & another) = default;

    GenericCell & operator+=(const ValueType & raw_value);
    GenericCell & operator=(const ValueType & raw_value);
    bool operator==(const GenericCell & another) const;
    bool operator==(const ValueType & another_value) const;

    // Another restriction here. T must overload operator++() as well
    // but not for operator++(int)
    GenericCell & operator++();

    // true if value in the range. Vacant is not valid.
    bool isValid() const;
    // true if it is vacant.
    bool isVacant() const;

    std::pair<unsigned int, unsigned int> getPosition() const;

    void getPosition(unsigned int & row, unsigned int & col) const;

    void setPosition(const int & i, const int & j);

    void reset();

    // must be explict because we already have operator=(const ValueType & raw_value);
    // if implicit, (cell + raw_value) may get confused...
    // use static_cast<ValueType>(cell) when you try to call this conversion operator.
    inline explicit operator ValueType() const
    {
      return value;
    }

    // another restriction on ValueType, it must overload operator<<
    friend std::ostream & operator<<(std::ostream & os, const GenericCell & cell)
    {
      os << cell.value;
      return os;
    }

    // another restriction on ValueType again, it must overload operator>>
    friend std::istream & operator>>(std::istream & is, GenericCell & cell)
    {
      is >> cell.value;
      return is;
    }

    // with explicit operator ValueType(), this function should return an GenericCel
    friend GenericCell operator+(const GenericCell & cell, const ValueType & raw_value)
    {
      GenericCell new_cell(cell);
      new_cell += raw_value;
      return new_cell;
    }

  private:
    Position position;
    T value;

  };

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH>::GenericCell(unsigned int x, unsigned int y, T raw_value) : position(x, y), value(raw_value) {}

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH>::GenericCell(T raw_value) : position(255, 255), value(raw_value) {}

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH> & GenericCell<T, MIN, WIDTH>::operator=(const ValueType & raw_value)
  {
    this->value = raw_value;
    return *this;
  }

  template<typename T, T MIN, unsigned int WIDTH>
  bool GenericCell<T, MIN, WIDTH>::operator==(const GenericCell & another) const
  {
    return (another.value == this->value);
  }

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH> & GenericCell<T, MIN, WIDTH>::operator+=(const ValueType & raw_value)
  {
    this->value += raw_value;
    return *this;
  }

  template<typename T, T MIN, unsigned int WIDTH>
  bool GenericCell<T, MIN, WIDTH>::operator==(const ValueType & another_value) const
  {
    return (another_value == value);
  }

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH> & GenericCell<T, MIN, WIDTH>::operator++()
  {
    ++value;
    return *this;
  }

  template<typename T, T MIN, unsigned int WIDTH>
  bool GenericCell<T, MIN, WIDTH>::isValid() const
  {
    return (value >= minimum_value - 1 && value <= maximum_value);
  }

  template<typename T, T MIN, unsigned int WIDTH>
  bool GenericCell<T, MIN, WIDTH>::isVacant() const
  {
    return (value <= minimum_value - 1);
  }

  template<typename T, T MIN, unsigned int WIDTH>
  Position GenericCell<T, MIN, WIDTH>::getPosition() const
  {
    return position;
  }

  template<typename T, T MIN, unsigned int WIDTH>
  void GenericCell<T, MIN, WIDTH>::getPosition(unsigned int & row, unsigned int & col) const
  {
    row = position.first;
    col = position.second;
  }

  template<typename T, T MIN, unsigned int WIDTH>
  void GenericCell<T, MIN, WIDTH>::setPosition(const int & i, const int & j)
  {
    position = Position{i, j};
  }

  template<typename T, T MIN, unsigned int WIDTH>
  void GenericCell<T, MIN, WIDTH>::reset()
  {
    value =  minimum_value - 1;
  }
}