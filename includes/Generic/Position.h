#pragma once

#include <functional>

namespace wubinboardgames
{
  typedef std::pair<unsigned int, unsigned int> Position;
  struct PositionHashser {
    std::size_t operator() (const Position & pos) const{
      auto row_hash = std::hash<unsigned int> {}(pos.first);
      auto col_hash = std::hash<unsigned int> {}(pos.second);
      return row_hash | (col_hash << 8);
    }
  };
  inline void CoordinateConvert(const Position & pos, unsigned int & row, unsigned int & col)
  {
    row = pos.first;
    col = pos.second;
  }
}