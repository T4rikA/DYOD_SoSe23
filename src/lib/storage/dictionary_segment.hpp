#pragma once

#include "abstract_segment.hpp"

namespace opossum {

class AbstractAttributeVector;

// Dictionary is a specific segment type that stores all its values in a vector
template <typename T>
class DictionarySegment : public AbstractSegment {
 public:
  /**
   * Creates a Dictionary segment from a given value segment.
   */
  explicit DictionarySegment(const std::shared_ptr<AbstractSegment>& abstract_segment);

  // Returns the value at a certain position. If you want to write efficient operators, back off!
  AllTypeVariant operator[](const ChunkOffset chunk_offset) const override;

  // Returns the value at a certain position. Throws an error if value is NULL.
  T get(const ChunkOffset chunk_offset) const;

  // Returns the value at a certain position. Returns std::nullopt if the value is NULL.
  std::optional<T> get_typed_value(const ChunkOffset chunk_offset) const;

  // Returns an underlying dictionary.
  const std::vector<T>& dictionary() const;

  // Returns an underlying data structure.
  std::shared_ptr<AbstractAttributeVector> attribute_vector() const;

  // Returns the ValueID used to represent a NULL value.
  ValueID null_value_id() const;

  // Returns the value represented by a given ValueID.
  const T value_of_value_id(const ValueID value_id) const;

  // Returns the first value ID that refers to a value >= the search value. Returns INVALID_VALUE_ID if all values are
  // smaller than the search value.
  ValueID lower_bound(const T value) const;

  // Same as lower_bound(T), but accepts an AllTypeVariant.
  ValueID lower_bound(const AllTypeVariant& value) const;

  // Returns the first value ID that refers to a value > the search value. Returns INVALID_VALUE_ID if all values are
  // smaller than or equal to the search value.
  ValueID upper_bound(const T value) const;

  // Same as upper_bound(T), but accepts an AllTypeVariant.
  ValueID upper_bound(const AllTypeVariant& value) const;

  // Returns the number of unique_values (dictionary entries).
  ChunkOffset unique_values_count() const;

  // Returns the number of entries.
  ChunkOffset size() const override;

  // Returns the calculated memory usage.
  size_t estimate_memory_usage() const final;

 protected:
  std::vector<T> _dictionary;
  std::shared_ptr<AbstractAttributeVector> _attribute_vector;
  bool _segment_nullable;
};

EXPLICITLY_DECLARE_DATA_TYPES(DictionarySegment);

}  // namespace opossum
