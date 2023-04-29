#pragma once

#include <memory>

#include "all_type_variant.hpp"
#include "types.hpp"

namespace opossum {

class BaseIndex;
class AbstractSegment;

// A chunk is a horizontal partition of a table. For each column in the table, it holds one segment. The segments
// across all chunks constitute the column.
//
// Find more information about this in our wiki: https://github.com/hyrise/hyrise/wiki/chunk-concept
class Chunk : private Noncopyable {
 public:
  // Creates an empty chunk.
  Chunk() = default;

  // Adds a segment to the "right" of the chunk.
  void add_segment(const std::shared_ptr<AbstractSegment> segment);

  // Returns the number of columns (cannot exceed ColumnID (uint16_t)).
  ColumnCount column_count() const;

  // Returns the number of rows (cannot exceed ChunkOffset (uint32_t)).
  ChunkOffset size() const;

  // Adds a new row, given as a list of values, to the chunk. Note this is slow and not thread-safe and should be used
  // for testing purposes only.
  void append(const std::vector<AllTypeVariant>& values);

  // Returns the segment at a given position.
  std::shared_ptr<AbstractSegment> get_segment(ColumnID column_id) const;

 protected:
  std::vector<std::shared_ptr<AbstractSegment>> _segments;
};

}  // namespace opossum
