#include "base_test.hpp"

#include "storage/value_segment.hpp"

namespace opossum {

class StorageValueSegmentTest : public BaseTest {
 protected:
  ValueSegment<int32_t> int_value_segment{true};
  ValueSegment<std::string> string_value_segment{};
  ValueSegment<double> double_value_segment{};
};

TEST_F(StorageValueSegmentTest, GetSize) {
  EXPECT_EQ(int_value_segment.size(), 0);
  EXPECT_EQ(string_value_segment.size(), 0);
  EXPECT_EQ(double_value_segment.size(), 0);
}

TEST_F(StorageValueSegmentTest, AppendValueOfSameType) {
  int_value_segment.append(3);
  EXPECT_EQ(int_value_segment.size(), 1);

  string_value_segment.append("Hello");
  EXPECT_EQ(string_value_segment.size(), 1);

  double_value_segment.append(3.14);
  EXPECT_EQ(double_value_segment.size(), 1);
}

TEST_F(StorageValueSegmentTest, AppendValueOfDifferentType) {
  int_value_segment.append(3.14);
  EXPECT_EQ(int_value_segment.size(), 1);
  EXPECT_THROW(int_value_segment.append("Hi"), std::logic_error);

  string_value_segment.append(3);
  string_value_segment.append(4.44);
  EXPECT_EQ(string_value_segment.size(), 2);

  double_value_segment.append(4);
  EXPECT_EQ(double_value_segment.size(), 1);
  EXPECT_THROW(double_value_segment.append("Hi"), std::logic_error);
}

TEST_F(StorageValueSegmentTest, AppendNullValue) {
  int_value_segment.append(NULL_VALUE);
  EXPECT_EQ(int_value_segment.size(), 1);

  EXPECT_THROW(string_value_segment.append(NULL_VALUE), std::logic_error);
  EXPECT_THROW(double_value_segment.append(NULL_VALUE), std::logic_error);
}

TEST_F(StorageValueSegmentTest, MemoryUsage) {
  int_value_segment.append(1);
  EXPECT_EQ(int_value_segment.estimate_memory_usage(), size_t{4});
  int_value_segment.append(2);
  EXPECT_EQ(int_value_segment.estimate_memory_usage(), size_t{8});
}

TEST_F(StorageValueSegmentTest, NullValueHandling) {
  int_value_segment.append(1);
  int_value_segment.append(NULL_VALUE);

  EXPECT_EQ(int_value_segment[0], AllTypeVariant{1});
  EXPECT_TRUE(variant_is_null(int_value_segment[1]));

  EXPECT_FALSE(int_value_segment.is_null(0));
  EXPECT_TRUE(int_value_segment.is_null(1));

  EXPECT_EQ(int_value_segment.get(0), 1);
  EXPECT_THROW(int_value_segment.get(1), std::logic_error);

  EXPECT_EQ(int_value_segment.get_typed_value(0), 1);
  EXPECT_EQ(int_value_segment.get_typed_value(1), std::nullopt);

  EXPECT_EQ(int_value_segment.values().size(), int_value_segment.null_values().size());
  EXPECT_EQ(int_value_segment.values().size(), 2);

  EXPECT_TRUE(int_value_segment.is_nullable());

  EXPECT_FALSE(string_value_segment.is_nullable());
  EXPECT_THROW(string_value_segment.null_values(), std::logic_error);
}

TEST_F(StorageValueSegmentTest, GetValues) {
  int_value_segment.append(1);
  int_value_segment.append(2);
  EXPECT_EQ(int_value_segment.values(), (std::vector<int>{1,2}));
  double_value_segment.append(0.0);
  EXPECT_EQ(double_value_segment.values(), (std::vector<double>{0.0}));
  EXPECT_EQ(string_value_segment.values(), (std::vector<std::string>{}));
}

TEST_F(StorageValueSegmentTest, IndexingOperator) {
  int_value_segment.append(1);
  int_value_segment.append(2);
  EXPECT_EQ(int_value_segment[0], AllTypeVariant{1});
  EXPECT_EQ(int_value_segment[1], AllTypeVariant{2});
}

TEST_F(StorageValueSegmentTest, CorrectNulling) {
  auto int_value_segment = ValueSegment<int32_t>{true};
  EXPECT_TRUE(int_value_segment.is_nullable());
  int_value_segment = ValueSegment<int32_t>{false};
  EXPECT_FALSE(int_value_segment.is_nullable());
}

}  // namespace opossum
