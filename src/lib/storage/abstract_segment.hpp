#pragma once

#include <memory>
#include <optional>

#include "all_type_variant.hpp"
#include "types.hpp"

namespace opossum {

// AbstractSegment is the abstract super class for all segment types, i.e, ValueSegment, DictionarySegment,
// ReferenceSegment.
class AbstractSegment : private Noncopyable {
 public:
  AbstractSegment() = default;
  virtual ~AbstractSegment() = default;

  // We need to explicitly set the move constructor to default when we overwrite the copy constructor.
  AbstractSegment(AbstractSegment&&) = default;

  AbstractSegment& operator=(AbstractSegment&&) = default;

  // Returns the value at a given position.
  virtual AllTypeVariant operator[](const ChunkOffset chunk_offset) const = 0;

  // Returns the number of values.
  virtual ChunkOffset size() const = 0;

  // Returns the calculated memory usage.
  virtual size_t estimate_memory_usage() const = 0;
};

}  // namespace opossum
