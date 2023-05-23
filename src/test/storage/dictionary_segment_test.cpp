#include "base_test.hpp"

#include "resolve_type.hpp"
#include "storage/abstract_attribute_vector.hpp"
#include "storage/abstract_segment.hpp"
#include "storage/dictionary_segment.hpp"

namespace opossum {

class StorageDictionarySegmentTest : public BaseTest {
 protected:
  std::shared_ptr<ValueSegment<int32_t>> value_segment_int{std::make_shared<ValueSegment<int32_t>>()};
  std::shared_ptr<ValueSegment<std::string>> value_segment_str{std::make_shared<ValueSegment<std::string>>(true)};
};

TEST_F(StorageDictionarySegmentTest, CompressSegmentString) {
  value_segment_str->append("Bill");
  value_segment_str->append("Steve");
  value_segment_str->append("Alexander");
  value_segment_str->append("Steve");
  value_segment_str->append("Hasso");
  value_segment_str->append("Bill");
  value_segment_str->append(NULL_VALUE);

  const auto dict_segment = std::make_shared<DictionarySegment<std::string>>(value_segment_str);

  // Test attribute_vector size.
  EXPECT_EQ(dict_segment->size(), 7);

  // Test dictionary size (uniqueness).
  EXPECT_EQ(dict_segment->unique_values_count(), 4);

  // Test sorting.
  const auto& dict = dict_segment->dictionary();
  EXPECT_EQ(dict[0], "Alexander");
  EXPECT_EQ(dict[1], "Bill");
  EXPECT_EQ(dict[2], "Hasso");
  EXPECT_EQ(dict[3], "Steve");

  // Test NULL value handling.
  EXPECT_EQ(dict_segment->attribute_vector()->get(6), dict_segment->null_value_id());
  EXPECT_EQ(dict_segment->get_typed_value(6), std::nullopt);
  EXPECT_THROW(dict_segment->get(6), std::logic_error);
}

TEST_F(StorageDictionarySegmentTest, LowerUpperBound) {
  for (auto value = int16_t{0}; value <= 10; value += 2) {
    value_segment_int->append(value);
  }

  std::shared_ptr<AbstractSegment> segment;
  resolve_data_type("int", [&](auto type) {
    using Type = typename decltype(type)::type;
    segment = std::make_shared<DictionarySegment<Type>>(value_segment_int);
  });
  auto dict_segment = std::dynamic_pointer_cast<DictionarySegment<int32_t>>(segment);

  EXPECT_EQ(dict_segment->lower_bound(4), ValueID{2});
  EXPECT_EQ(dict_segment->upper_bound(4), ValueID{3});

  EXPECT_EQ(dict_segment->lower_bound(AllTypeVariant{4}), ValueID{2});
  EXPECT_EQ(dict_segment->upper_bound(AllTypeVariant{4}), ValueID{3});

  EXPECT_EQ(dict_segment->lower_bound(5), ValueID{3});
  EXPECT_EQ(dict_segment->upper_bound(5), ValueID{3});

  EXPECT_EQ(dict_segment->lower_bound(15), INVALID_VALUE_ID);
  EXPECT_EQ(dict_segment->upper_bound(15), INVALID_VALUE_ID);
}

TEST_F(StorageDictionarySegmentTest, AccessOperators) {
  value_segment_str->append("Bill");
  value_segment_str->append("Hasso");
  value_segment_str->append(NULL_VALUE);

  const auto dict_segment = std::make_shared<DictionarySegment<std::string>>(value_segment_str);

  EXPECT_EQ(dict_segment->operator[](0), static_cast<AllTypeVariant>("Bill"));
  EXPECT_EQ(dict_segment->operator[](1), static_cast<AllTypeVariant>("Hasso"));
  EXPECT_TRUE(variant_is_null(dict_segment->operator[](2)));

  EXPECT_EQ(dict_segment->get_typed_value(0), "Bill");
  EXPECT_EQ(dict_segment->get_typed_value(1), "Hasso");

  EXPECT_EQ(dict_segment->get(0), "Bill");
  EXPECT_EQ(dict_segment->get(1), "Hasso");
}

TEST_F(StorageDictionarySegmentTest, ValueOfValueID) {
  value_segment_str->append("Bill");
  value_segment_str->append("Hasso");
  value_segment_str->append(NULL_VALUE);

  const auto dict_segment = std::make_shared<DictionarySegment<std::string>>(value_segment_str);

  EXPECT_EQ(dict_segment->value_of_value_id(ValueID{1}), std::string("Bill"));
  EXPECT_EQ(dict_segment->value_of_value_id(ValueID{2}), std::string("Hasso"));

  EXPECT_THROW(dict_segment->value_of_value_id(dict_segment->null_value_id()), std::logic_error);
}

TEST_F(StorageDictionarySegmentTest, MemoryUsageString) {
  value_segment_str->append("Hello");
  const auto column_str = std::make_shared<DictionarySegment<std::string>>(value_segment_str);
  const auto dict_col_str = std::dynamic_pointer_cast<opossum::DictionarySegment<std::string>>(column_str);

  EXPECT_EQ(dict_col_str->estimate_memory_usage(), 1 * sizeof(std::string) + 1 * sizeof(uint8_t));
}

TEST_F(StorageDictionarySegmentTest, MemoryUsageUInt8) {
  for (auto index = int8_t{0}; index < 100; ++index) {
    value_segment_int->append(index);
  }
  const auto column = std::make_shared<DictionarySegment<int32_t>>(value_segment_int);
  const auto dict_col = std::dynamic_pointer_cast<opossum::DictionarySegment<int32_t>>(column);

  EXPECT_EQ(dict_col->estimate_memory_usage(), 100 * sizeof(int32_t) + 100 * sizeof(uint8_t));
}

TEST_F(StorageDictionarySegmentTest, MemoryUsageUInt16) {
  for (auto index = int16_t{0}; index < UINT8_MAX + 2; ++index) {
    value_segment_int->append(index);
  }
  const auto column = std::make_shared<DictionarySegment<int32_t>>(value_segment_int);
  const auto dict_col = std::dynamic_pointer_cast<opossum::DictionarySegment<int32_t>>(column);

  EXPECT_EQ(dict_col->estimate_memory_usage(), (UINT8_MAX + 2) * sizeof(int32_t) + (UINT8_MAX + 2) * sizeof(uint16_t));
}

TEST_F(StorageDictionarySegmentTest, MemoryUsageUInt32) {
  for (auto index = int32_t{0}; index < (UINT16_MAX + 2); ++index) {
    value_segment_int->append(index);
  }

  auto column = std::make_shared<DictionarySegment<int32_t>>(value_segment_int);
  auto dict_col = std::dynamic_pointer_cast<opossum::DictionarySegment<int32_t>>(column);

  EXPECT_EQ(dict_col->estimate_memory_usage(),
            ((UINT16_MAX + 2)) * sizeof(int32_t) + ((UINT16_MAX + 2)) * sizeof(uint32_t));
}

}  // namespace opossum
