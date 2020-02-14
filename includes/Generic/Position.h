#pragma once

#include <utility> // pair
#include <functional> // std::hash

namespace wubinboardgames
{
  typedef std::pair<unsigned int, unsigned int> Position;
  // std::pair has no explicit specilization of std::hash...
  // to make it hashable by unordered_map. I made this one.
  // Since width usually is less than 255. left shift 8 should
  // gurantee each Position has a unique hashcode.
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