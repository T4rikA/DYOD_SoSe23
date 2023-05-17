#pragma once

#include "abstract_attribute_vector.hpp"
#include "types.hpp"

namespace opossum {

template <typename T>
class FixedWidthIntegerVector : public AbstractAttributeVector {
 public:
  // mark as explicit when just one value is passed
  FixedWidthIntegerVector() = default;
  ~FixedWidthIntegerVector() = default;

  // We need to explicitly set the move constructor to default when we overwrite the copy constructor.
  // FixedWidthAttributeVector(FixedWidthAttributeVector&&) = default;
  // FixedWidthAttributeVector& operator=(FixedWidthAttributeVector&&) = default;

  // Returns the value id at a given position.
  ValueID get(const size_t index) const override;

  // Sets the value id at a given position.
  void set(const size_t index, const ValueID value_id) override;

  // Returns the number of values.
  size_t size() const override;

  // Returns the width of biggest value id in bytes.
  FixedWidthIntegerVector width() const override;
};

}  // namespace opossum
