#pragma once

#include "abstract_attribute_vector.hpp"
#include "types.hpp"

namespace opossum {

template <typename uintX_t>
class FixedWidthIntegerVector : public AbstractAttributeVector {
 public:
  explicit FixedWidthIntegerVector(size_t size);

  // Returns the value id at a given position.
  ValueID get(const size_t index) const override;

  // Sets the value id at a given position.
  void set(const size_t index, const ValueID value_id) override;

  // Returns the number of values.
  size_t size() const override;

  // Returns the width of biggest value id in bytes.
  AttributeVectorWidth width() const override;

 private:
  std::vector<uintX_t> _values;
};

extern template class FixedWidthIntegerVector<uint8_t>;
extern template class FixedWidthIntegerVector<uint16_t>;
extern template class FixedWidthIntegerVector<uint32_t>;

}  // namespace opossum
