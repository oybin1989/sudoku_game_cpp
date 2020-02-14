#pragma once

#include <iostream>

#include "Position.h"

namespace wubinboardgames
{
  template<typename T, T MIN, unsigned int WIDTH>
  struct GenericCell
  {
    static constexpr T minimum_value = MIN;
    static constexpr T maximum_value = MIN + WIDTH - 1;
    static constexpr T values_length = WIDTH;
    typedef T ValueType;

    GenericCell(unsigned int x, unsigned int y, T value=MIN-1);
    GenericCell(T=MIN-1);
    // rule of five
    virtual ~GenericCell() = default;
    GenericCell(const GenericCell & another) = default;
    GenericCell(GenericCell && another) = default;
    GenericCell & operator=(GenericCell && another) = default;
    GenericCell & operator=(const GenericCell & another) = default;
    GenericCell & operator+=(const ValueType & value);
    GenericCell & operator=(const ValueType & value);
    bool operator==(const GenericCell & another) const;
    bool operator==(const ValueType & another_value) const;
    GenericCell & operator++();
    bool isValid() const;
    bool isVacant() const;
    std::pair<unsigned int, unsigned int> getPosition() const;
    void getPosition(unsigned int & row, unsigned int & col) const;
    void setPosition(const int & i, const int & j);
    void reset();
    inline explicit operator T() const
    {
      return value;
    }
    friend std::ostream & operator<<(std::ostream & os, const GenericCell & cell)
    {
      os << cell.value;
      return os;
    }
    friend std::istream & operator>>(std::istream & is, GenericCell & cell)
    {
      is >> cell.value;
      return is;
    }
    friend GenericCell operator+(const GenericCell & cell, const ValueType & value)
    {
      GenericCell new_cell(cell);
      new_cell += value;
      return new_cell;
    }
  private:
    Position position;
    T value;
  };

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH>::GenericCell(unsigned int x, unsigned int y, T value) : position(x, y), value(value) {}

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH>::GenericCell(T value) : position(255, 255), value(value) {}

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH> & GenericCell<T, MIN, WIDTH>::operator=(const ValueType & value)
  {
    this->value = value;
    return *this;
  }

  template<typename T, T MIN, unsigned int WIDTH>
  bool GenericCell<T, MIN, WIDTH>::operator==(const GenericCell & another) const
  {
    return (another.value == this->value);
  }

  template<typename T, T MIN, unsigned int WIDTH>
  GenericCell<T, MIN, WIDTH> & GenericCell<T, MIN, WIDTH>::operator+=(const ValueType & value)
  {
    this->value += value;
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