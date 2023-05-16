#include "fixed_width_integer_vector.hpp"

namespace opossum {

template <typename T>
FixedWidthIntegerVector<T>::FixedWidthIntegerVector(size_t size) : _values(size) {}

template <typename T>
ValueID FixedWidthIntegerVector<T>::get(const size_t index) const {
  return ValueID{_values.at(index)};
}

template <typename T>
void FixedWidthIntegerVector<T>::set(const size_t index, const ValueID value_id) {
  _values.at(index) = value_id;
}

template <typename T>
size_t FixedWidthIntegerVector<T>::size() const {
  return _values.size();
}

template <typename T>
AttributeVectorWidth FixedWidthIntegerVector<T>::width() const {
  return sizeof(T);
}

template class FixedWidthIntegerVector<uint8_t>;
template class FixedWidthIntegerVector<uint16_t>;
template class FixedWidthIntegerVector<uint32_t>;

}  // namespace opossum
