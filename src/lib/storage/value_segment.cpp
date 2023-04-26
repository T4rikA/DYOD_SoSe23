#include "value_segment.hpp"

#include "type_cast.hpp"
#include "utils/assert.hpp"

namespace opossum {

template <typename T>
ValueSegment<T>::ValueSegment(bool nullable) {
  _values = std::vector<T>(0);
  _is_null = std::vector<bool>(0);
  _nullable = nullable;
}

template <typename T>
AllTypeVariant ValueSegment<T>::operator[](const ChunkOffset chunk_offset) const {
  if (_is_null[chunk_offset]) {
    return NULL_VALUE;
  }
  return _values[chunk_offset];
}

template <typename T>
bool ValueSegment<T>::is_null(const ChunkOffset chunk_offset) const {
  return _is_null[chunk_offset];
}

template <typename T>
T ValueSegment<T>::get(const ChunkOffset chunk_offset) const {
  Assert(!_is_null[chunk_offset], "Can't return NULL Value!");
  return _values[chunk_offset];
}

template <typename T>
std::optional<T> ValueSegment<T>::get_typed_value(const ChunkOffset chunk_offset) const {
  if (_is_null[chunk_offset]) {
    return std::nullopt;
  }
  return _values[chunk_offset];
}

template <typename T>
void ValueSegment<T>::append(const AllTypeVariant& value) {
  if (variant_is_null(value)) {
    Assert(_nullable, "Tried to insert NULL value in not nullable segment!");
    _values.push_back(type_cast<T>(0));
    _is_null.push_back(true);
  } else {
    //Assert(std::is_arithmetic_v(value) || std::is_same_v(T, typeid(value).name()),"Cannot convert String to numerical");
    _values.push_back(type_cast<T>(value));
    _is_null.push_back(false);
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
  return _nullable;
}

template <typename T>
const std::vector<bool>& ValueSegment<T>::null_values() const {
  if (!_nullable) {
    throw std::logic_error("Segment is not nullable so there are no NULL values!");
  }
  return _is_null;
}

template <typename T>
size_t ValueSegment<T>::estimate_memory_usage() const {
  return _values.size() * sizeof(T);
}

// Macro to instantiate the following classes:
// template class ValueSegment<int32_t>;
// template class ValueSegment<int64_t>;
// template class ValueSegment<float>;
// template class ValueSegment<double>;
// template class ValueSegment<std::string>;
EXPLICITLY_INSTANTIATE_DATA_TYPES(ValueSegment);

}  // namespace opossum
