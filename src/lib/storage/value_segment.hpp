#pragma once

#include "abstract_segment.hpp"

namespace opossum {

// ValueSegment is a segment type that stores all its values in a vector.
template <typename T>
class ValueSegment : public AbstractSegment {
 public:
  explicit ValueSegment(bool nullable = false);

  // Returns the value at a certain position. If you want to write efficient operators, back off!
  AllTypeVariant operator[](const ChunkOffset chunk_offset) const final;

  // Returns whether a value is NULL.
  bool is_null(const ChunkOffset chunk_offset) const;

  // Returns the value at a certain position. Throws an error if value is NULL.
  T get(const ChunkOffset chunk_offset) const;

  // Returns the value at a certain position. Returns std::nullopt if the value is NULL.
  std::optional<T> get_typed_value(const ChunkOffset chunk_offset) const;

  // Adds a value at the end of the segment.
  void append(const AllTypeVariant& value);

  // Returns the number of entries.
  ChunkOffset size() const final;

  // Returns all values. This is the preferred method to check a value at a certain index. Usually you need to access
  // more than a single value anyway.
  // e.g. const auto& values = value_segment.values(); and then: values[i]; in your loop.
  const std::vector<T>& values() const;

  // Returns whether segment supports NULL values.
  bool is_nullable() const;

  // Returns NULL value vector that indicates whether a value is NULL with true at position i. Throw an exception if
  // is_nullable() returns false. This is the preferred method to check for a NULL value at a certain index. Usually
  // you need to access more than a single value anyway.
  const std::vector<bool>& null_values() const;

  // Returns the calculated memory usage.
  size_t estimate_memory_usage() const final;

 protected:
    std::vector<T> _value_vector;
    std::vector<bool> _is_null_vector;
    bool _segment_is_nullable;
};

EXPLICITLY_DECLARE_DATA_TYPES(ValueSegment);

}  // namespace opossum
