#include "fixed_width_integer_vector.hpp"
#include "utils/assert.hpp"

namespace opossum {

template <typename uintX_t>
FixedWidthIntegerVector<uintX_t>::FixedWidthIntegerVector(size_t size) : _values(size) {}

template <typename uintX_t>
ValueID FixedWidthIntegerVector<uintX_t>::get(const size_t index) const {
  return ValueID{_values.at(index)};
}

template <typename uintX_t>
void FixedWidthIntegerVector<uintX_t>::set(const size_t index, const ValueID value_id) {
  DebugAssert(index < size(), "index " + std::to_string(index) +
                                  " out of bounds for FixedWidthIntegerVector with size " + std::to_string(size()));
  _values.at(index) = value_id;
}

template <typename uintX_t>
size_t FixedWidthIntegerVector<uintX_t>::size() const {
  return _values.size();
}

template <typename uintX_t>
AttributeVectorWidth FixedWidthIntegerVector<uintX_t>::width() const {
  return sizeof(uintX_t);
}

template class FixedWidthIntegerVector<uint8_t>;
template class FixedWidthIntegerVector<uint16_t>;
template class FixedWidthIntegerVector<uint32_t>;

}  // namespace opossum
