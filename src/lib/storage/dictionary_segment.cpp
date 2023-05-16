#include "dictionary_segment.hpp"

#include <set>
#include <unordered_map>
#include "type_cast.hpp"
#include "utils/assert.hpp"
#include "value_segment.hpp"

namespace opossum {

template <typename T>
DictionarySegment<T>::DictionarySegment(const std::shared_ptr<AbstractSegment>& abstract_segment) {
  // retrieve value segment
  const auto value_segment = std::dynamic_pointer_cast<ValueSegment<T>>(abstract_segment);
  Assert(value_segment, "Cant encode abstract segment, because it is no value segment.");

  auto values = value_segment->values();
  auto value_segment_size = value_segment->size();

  auto unique_values = std::unordered_map<T, ValueID>();
  // do for different sizes

  //TODO this is dependent on null_value_id() being 0
  auto last_index = 1;
  for (auto index = size_t{0}; index < value_segment_size; ++index) {
    if (value_segment->is_null(index)) {
      unique_values[values[index]] = null_value_id();
    } else {
      if (unique_values.find(values[index]) != unique_values.end()) {
        unique_values[values[index]] = last_index;
        ++last_index;
      }
    }
  }
  
  //for (auto value : unique_values) {}

  //moved this out since once adjust the intsize to the number of values we deal with we cannot construct the compressed_values vector before knowing the amount of unique value
  auto compressed_values = std::vector<uint32_t>(value_segment_size);

  /*
  // determine unique values for dictionary
  _attribute_vector = std::make_shared<std::vector<uint32_t>>();
  std::set<T> unique_values = std::set<T>();
  auto segment_size = abstract_segment->size();
  for (auto value_index = size_t{0}; value_index < segment_size; ++value_index) {
    unique_values.insert(type_cast<T>((*abstract_segment)[value_index]));
  }
  // build dictionary using unique values and collect indices for values in hashmap of value->index
  auto value_positions = std::unordered_map<T, ValueID>();
  // we use unique value id 0 for null values
  auto unique_values_index = uint32_t{1};
  for (auto value : unique_values) {
    // TODO maybe use unique value count to resize vector and place at index instead of push back?
    _dictionary.push_back(type_cast<T>(value));
    value_positions[type_cast<T>(value)] = unique_values_index;
    unique_values_index++;
  }
  // apply dict enconding
  for (auto attribute_index = size_t{0}; attribute_index < segment_size; ++attribute_index) {
    auto value = type_cast<T>((*abstract_segment)[attribute_index]);
    auto dict_value = value_positions.at(type_cast<T>(value));
    _attribute_vector->push_back(dict_value);
  }*/
  return;
}

template <typename T>
AllTypeVariant DictionarySegment<T>::operator[](const ChunkOffset chunk_offset) const {
  auto return_value = get_typed_value(chunk_offset);
  if(return_value){
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
  const auto value_id = static_cast<ValueID>(attribute_vector()->at(chunk_offset));
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
std::shared_ptr<const std::vector<uint32_t>> DictionarySegment<T>::attribute_vector() const {
  return _attribute_vector;
}

template <typename T>
ValueID DictionarySegment<T>::null_value_id() const {
  return static_cast<ValueID>(0);
}

template <typename T>
const T DictionarySegment<T>::value_of_value_id(const ValueID value_id) const {
  Assert(value_id != null_value_id(), "Can't retrieve value for null value.");
  return dictionary().at(value_id-1);
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
  return dictionary().size();
}

template <typename T>
ChunkOffset DictionarySegment<T>::size() const {
  return static_cast<ChunkOffset>(_attribute_vector->size());
}

template <typename T>
size_t DictionarySegment<T>::estimate_memory_usage() const {
  return size_t{dictionary().size() * sizeof(uint32_t) /*+ _attribute_vector.size() * sizeof (T)*/};
}

EXPLICITLY_INSTANTIATE_DATA_TYPES(DictionarySegment);

}  // namespace opossum
