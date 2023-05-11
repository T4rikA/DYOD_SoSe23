#include "base_test.hpp"

#include "resolve_type.hpp"
#include "storage/abstract_segment.hpp"
#include "storage/chunk.hpp"

namespace opossum {

class StorageChunkTest : public BaseTest {
 protected:
  void SetUp() override {
    int32_value_segment = std::make_shared<ValueSegment<int32_t>>();
    int32_value_segment->append(4);
    int32_value_segment->append(6);
    int32_value_segment->append(3);

    int64_value_segment = std::make_shared<ValueSegment<int64_t>>();
    int64_value_segment->append(4);
    int64_value_segment->append(6);
    int64_value_segment->append(3);

    float_value_segment = std::make_shared<ValueSegment<float>>();
    float_value_segment->append(4.0);
    float_value_segment->append(6.0);
    float_value_segment->append(3.0);

    double_value_segment = std::make_shared<ValueSegment<double>>();
    double_value_segment->append(4.0);
    double_value_segment->append(6.0);
    double_value_segment->append(3.0);

    string_value_segment = std::make_shared<ValueSegment<std::string>>();
    string_value_segment->append("Hello,");
    string_value_segment->append("world");
    string_value_segment->append("!");
  }

  Chunk chunk;
  std::shared_ptr<ValueSegment<int32_t>> int32_value_segment{};
  std::shared_ptr<ValueSegment<int64_t>> int64_value_segment{};
  std::shared_ptr<ValueSegment<std::string>> string_value_segment{};
  std::shared_ptr<ValueSegment<float>> float_value_segment{};
  std::shared_ptr<ValueSegment<double>> double_value_segment{};
};

TEST_F(StorageChunkTest, AddSegmentToChunk) {
  EXPECT_EQ(chunk.size(), 0);
  chunk.add_segment(int32_value_segment);
  chunk.add_segment(string_value_segment);
  EXPECT_EQ(chunk.size(), 3);
}

TEST_F(StorageChunkTest, AddValuesToChunk) {
  chunk.add_segment(int32_value_segment);
  chunk.add_segment(int64_value_segment);
  chunk.add_segment(string_value_segment);
  chunk.add_segment(float_value_segment);
  chunk.add_segment(double_value_segment);
  chunk.append({2, 7, "two", 20.5, 7.2});
  EXPECT_EQ(chunk.size(), 4);

  if constexpr (OPOSSUM_DEBUG) {
    EXPECT_THROW(chunk.append({}), std::logic_error);
    EXPECT_THROW(chunk.append({4, "val", 3}), std::logic_error);
    EXPECT_EQ(chunk.size(), 4);
  }
}

TEST_F(StorageChunkTest, RetrieveSegment) {
  chunk.add_segment(int32_value_segment);
  chunk.add_segment(string_value_segment);
  chunk.append({2, "two"});

  auto segment = chunk.get_segment(ColumnID{0});
  EXPECT_EQ(segment->size(), 4);
}

TEST_F(StorageChunkTest, ColumnCount) {
  EXPECT_EQ(chunk.column_count(), 0u);
  chunk.add_segment(int32_value_segment);
  EXPECT_EQ(chunk.column_count(), 1u);
  chunk.add_segment(string_value_segment);
  EXPECT_EQ(chunk.column_count(), 2u);
}

}  // namespace opossum
