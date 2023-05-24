#include "base_test.hpp"

#include "storage/fixed_width_integer_vector.hpp"

namespace opossum {

class FixedWidthIntegerVectorTest : public BaseTest {
 protected:
  size_t element_count = 4;
  std::shared_ptr<FixedWidthIntegerVector<uint8_t>> small_vector{
      std::make_shared<FixedWidthIntegerVector<uint8_t>>(element_count)};
  std::shared_ptr<FixedWidthIntegerVector<uint16_t>> medium_vector{
      std::make_shared<FixedWidthIntegerVector<uint16_t>>(element_count)};
  std::shared_ptr<FixedWidthIntegerVector<uint32_t>> large_vector{
      std::make_shared<FixedWidthIntegerVector<uint32_t>>(element_count)};
};

TEST_F(FixedWidthIntegerVectorTest, BasicOperations) {
  for (auto index = size_t{0}; index < element_count; ++index) {
    small_vector->set(index, static_cast<ValueID>(index));
    medium_vector->set(index, static_cast<ValueID>(index));
    large_vector->set(index, static_cast<ValueID>(index));
  }

  for (auto index = size_t{0}; index < element_count; ++index) {
    EXPECT_EQ(small_vector->get(index), static_cast<ValueID>(index));
    EXPECT_EQ(medium_vector->get(index), static_cast<ValueID>(index));
    EXPECT_EQ(large_vector->get(index), static_cast<ValueID>(index));
  }

  EXPECT_EQ(small_vector->size(), element_count);
  EXPECT_EQ(medium_vector->size(), element_count);
  EXPECT_EQ(large_vector->size(), element_count);

  EXPECT_EQ(small_vector->width(), sizeof(uint8_t));
  EXPECT_EQ(medium_vector->width(), sizeof(uint16_t));
  EXPECT_EQ(large_vector->width(), sizeof(uint32_t));
}

}  // namespace opossum
