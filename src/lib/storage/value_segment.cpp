#include "value_segment.hpp"

#include "type_cast.hpp"
#include "utils/assert.hpp"

namespace opossum {

template <typename T>
ValueSegment<T>::ValueSegment(bool nullable) : _values{}, _is_null_values{}, _segment_is_nullable(nullable) {}

template <typename T>
AllTypeVariant ValueSegment<T>::operator[](const ChunkOffset chunk_offset) const {
  if (is_null(chunk_offset)) {
    return NULL_VALUE;
  }
  return get(chunk_offset);
}

template <typename T>
bool ValueSegment<T>::is_null(const ChunkOffset chunk_offset) const {
  return is_nullable() && _is_null_values.at(chunk_offset);
}

template <typename T>
T ValueSegment<T>::get(const ChunkOffset chunk_offset) const {
  Assert(!is_null(chunk_offset), "Chunk is null, can't return value.");
  return _values[chunk_offset];
}

template <typename T>
std::optional<T> ValueSegment<T>::get_typed_value(const ChunkOffset chunk_offset) const {
  if (is_null(chunk_offset)) {
    return std::nullopt;
  }
  return get(chunk_offset);
}

template <typename T>
void ValueSegment<T>::append(const AllTypeVariant& value) {
  if (variant_is_null(value)) {
    Assert(_segment_is_nullable, "Tried to insert NULL value in not nullable segment!");
    _values.push_back(type_cast<T>(0));
    _is_null_values.push_back(true);
  } else {
    try {
      _values.push_back(type_cast<T>(value));
      _is_null_values.push_back(false);
    } catch (...) {
      throw std::logic_error{"Wrong argument type"};
    }
  }
}

template <typename T>
ChunkOffset ValueSegment<T>::size() const {
  return _values.size();
}

template <typename T>
const std::vector<T>& ValueSegment<T>::values() const {
  return _values;
}

template <typename T>
bool ValueSegment<T>::is_nullable() const {
  return _segment_is_nullable;
}

template <typename T>
const std::vector<bool>& ValueSegment<T>::null_values() const {
  Assert(is_nullable(), "Segment is not nullable, so can't retrieve null values.");
  return _is_null_values;
}

template <typename T>
size_t ValueSegment<T>::estimate_memory_usage() const {
  return _values.capacity() * sizeof(T);
}

// Macro to instantiate the following classes:
// template class ValueSegment<int32_t>;
// template class ValueSegment<int64_t>;
// template class ValueSegment<float>;
// template class ValueSegment<double>;
// template class ValueSegment<std::string>;
EXPLICITLY_INSTANTIATE_DATA_TYPES(ValueSegment);

}  // namespace opossum
