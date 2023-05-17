#include "dictionary_segment.hpp"

#include <map>
#include <set>
#include "fixed_width_integer_vector.hpp"
#include "type_cast.hpp"
#include "utils/assert.hpp"
#include "value_segment.hpp"

namespace opossum {

std::shared_ptr<AbstractAttributeVector> get_attribute_vector(size_t vector_size, size_t size) {
  auto bits_needed = std::bit_width(vector_size - 1);
  Assert(bits_needed <= 32, "Too many values in dictionary, cant use more than 32 bits!");
  if (bits_needed <= 8) {
    return std::make_shared<FixedWidthIntegerVector<uint8_t>>(size);
  } else if (bits_needed <= 16) {
    return std::make_shared<FixedWidthIntegerVector<uint16_t>>(size);
  } else {
    return std::make_shared<FixedWidthIntegerVector<uint32_t>>(size);
  }
}

template <typename T>
DictionarySegment<T>::DictionarySegment(const std::shared_ptr<AbstractSegment>& abstract_segment) {
  // retrieve value segment
  const auto value_segment = std::dynamic_pointer_cast<ValueSegment<T>>(abstract_segment);
  Assert(value_segment, "Can't encode abstract segment, because it is no value segment.");

  auto values = value_segment->values();
  auto value_segment_size = value_segment->size();
  _segment_nullable = value_segment->is_nullable();

  // stores value -> id
  auto unique_values = std::map<T, ValueID>();

  auto last_index = _segment_nullable ? 1 : 0;
  for (auto index = size_t{0}; index < value_segment_size; ++index) {
    if (!value_segment->is_null(index)) {
      auto value = values[index];
      if (unique_values.find(value) == unique_values.end()) {
        unique_values[value] = last_index;
        ++last_index;
      }
    }
  }

  for (auto [value, key] : unique_values) {
    _dictionary.push_back(value);
  }

  const auto attribute_vector = get_attribute_vector(unique_values.size(), value_segment_size);

  for (auto index = size_t{0}; index < value_segment_size; ++index) {
    if (value_segment->is_null(index)) {
      attribute_vector->set(index, null_value_id());
    } else {
      auto dict_value = unique_values[values[index]];
      attribute_vector->set(index, dict_value);
    }
  }
  _attribute_vector = attribute_vector;
  return;
}

template <typename T>
AllTypeVariant DictionarySegment<T>::operator[](const ChunkOffset chunk_offset) const {
  auto return_value = get_typed_value(chunk_offset);
  if (return_value) {
    return *return_value;
  }
  return NULL_VALUE;
}

template <typename T>
T DictionarySegment<T>::get(const ChunkOffset chunk_offset) const {
  const auto return_value = get_typed_value(chunk_offset);
  Assert(return_value.has_value(), "Value at position: " + std::to_string(chunk_offset) + "is NULL!");
  return *return_value;
}

template <typename T>
std::optional<T> DictionarySegment<T>::get_typed_value(const ChunkOffset chunk_offset) const {
  const auto value_id = attribute_vector()->get(chunk_offset);
  if (value_id == null_value_id()) {
    return std::nullopt;
  }
  return value_of_value_id(value_id);
}

template <typename T>
const std::vector<T>& DictionarySegment<T>::dictionary() const {
  return _dictionary;
}

template <typename T>
std::shared_ptr<AbstractAttributeVector> DictionarySegment<T>::attribute_vector() const {
  return _attribute_vector;
}

template <typename T>
ValueID DictionarySegment<T>::null_value_id() const {
  return _segment_nullable ? static_cast<ValueID>(0) : INVALID_VALUE_ID;
}

template <typename T>
const T DictionarySegment<T>::value_of_value_id(const ValueID value_id) const {
  Assert(_segment_nullable && value_id != null_value_id(), "Can't retrieve value for null value.");
  return dictionary().at(value_id - (_segment_nullable ? 1 : 0));
}

template <typename T>
ValueID DictionarySegment<T>::lower_bound(const T value) const {
  auto lower_bound_iterator = std::lower_bound(dictionary().begin(), dictionary().end(), value);
  if (lower_bound_iterator == dictionary().end()) {
    return INVALID_VALUE_ID;
  }
  return static_cast<ValueID>(std::distance(dictionary().begin(), lower_bound_iterator));
}

template <typename T>
ValueID DictionarySegment<T>::lower_bound(const AllTypeVariant& value) const {
  return lower_bound(type_cast<T>(value));
}

template <typename T>
ValueID DictionarySegment<T>::upper_bound(const T value) const {
  auto upper_bound_iterator = std::upper_bound(dictionary().begin(), dictionary().end(), value);
  if (upper_bound_iterator == dictionary().end()) {
    return INVALID_VALUE_ID;
  }
  return static_cast<ValueID>(std::distance(dictionary().begin(), upper_bound_iterator));
}

template <typename T>
ValueID DictionarySegment<T>::upper_bound(const AllTypeVariant& value) const {
  return upper_bound(type_cast<T>(value));
}

template <typename T>
ChunkOffset DictionarySegment<T>::unique_values_count() const {
  return dictionary().size();
}

template <typename T>
ChunkOffset DictionarySegment<T>::size() const {
  return static_cast<ChunkOffset>(attribute_vector()->size());
}

template <typename T>
size_t DictionarySegment<T>::estimate_memory_usage() const {
  auto dict_size = sizeof(T) * dictionary().size();
  auto att_vec_size = attribute_vector()->width() * attribute_vector()->size();
  return dict_size + att_vec_size;
}

EXPLICITLY_INSTANTIATE_DATA_TYPES(DictionarySegment);

}  // namespace opossum
