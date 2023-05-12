#include "dictionary_segment.hpp"

#include "utils/assert.hpp"
#include <set>
#include <unordered_map>
#include "type_cast.hpp"

namespace opossum {

template <typename T>
DictionarySegment<T>::DictionarySegment(const std::shared_ptr<AbstractSegment>& abstract_segment) {
/*  _attribute_vector = std::make_shared<std::vector<uint32_t>>();
  auto segment_size = abstract_segment->size();
  const auto& input_values = abstract_segment->values();
  const auto unique_values = std::set<T>{input_values.cbegin(), input_values.cend()};*/
  //
  // determine unique values for dictionary
  _attribute_vector = std::make_shared<std::vector<uint32_t>>();
  std::set<T> unique_values = std::set<T>();
  auto segment_size = abstract_segment->size();
  for (auto value_index = size_t{0}; value_index < segment_size; ++value_index) {
    unique_values.insert( type_cast<T>((*abstract_segment)[value_index]));
  }
  // build dictionary using unique values and collect indices for values in hashmap of value->index
  auto value_positions = std::unordered_map<T, uint32_t>();
  auto unique_values_index = uint32_t{0};
  for (const auto value : unique_values) {
    // TODO maybe use unique value count to resize vector and place at index instead of push back?
    _dictionary.push_back(type_cast<T>(value));
    value_positions[ type_cast<T>(value)] = unique_values_index;
    unique_values_index++;
  }
  // apply dict enconding
  for (auto attribute_index = size_t{0}; attribute_index < segment_size; ++attribute_index) {
    auto value = type_cast<T>((*abstract_segment)[attribute_index]);
    auto dict_value = value_positions.at(type_cast<T>(value));
    _attribute_vector->push_back(dict_value);
  }
  return;
}

template <typename T>
AllTypeVariant DictionarySegment<T>::operator[](const ChunkOffset chunk_offset) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
T DictionarySegment<T>::get(const ChunkOffset chunk_offset) const {
  return _dictionary[_attribute_vector->at(chunk_offset)];
}

template <typename T>
std::optional<T> DictionarySegment<T>::get_typed_value(const ChunkOffset chunk_offset) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
const std::vector<T>& DictionarySegment<T>::dictionary() const {
  return _dictionary;
}

template <typename T>
std::shared_ptr<const std::vector<uint32_t>> DictionarySegment<T>::attribute_vector() const {
  return _attribute_vector;
}

template <typename T>
ValueID DictionarySegment<T>::null_value_id() const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
const T DictionarySegment<T>::value_of_value_id(const ValueID value_id) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
ValueID DictionarySegment<T>::lower_bound(const T value) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
ValueID DictionarySegment<T>::lower_bound(const AllTypeVariant& value) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
ValueID DictionarySegment<T>::upper_bound(const T value) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
ValueID DictionarySegment<T>::upper_bound(const AllTypeVariant& value) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

template <typename T>
ChunkOffset DictionarySegment<T>::unique_values_count() const {
  return _dictionary.size();
}

template <typename T>
ChunkOffset DictionarySegment<T>::size() const {
  return static_cast<ChunkOffset>(_attribute_vector->size());
}

template <typename T>
size_t DictionarySegment<T>::estimate_memory_usage() const {
  return size_t{_dictionary.size() * sizeof (uint32_t) /*+ _attribute_vector.size() * sizeof (T)*/};
}

EXPLICITLY_INSTANTIATE_DATA_TYPES(DictionarySegment);

}  // namespace opossum
