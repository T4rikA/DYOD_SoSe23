#include "base_test.hpp"

#include "storage/table.hpp"

namespace opossum {

class StorageTableTest : public BaseTest {
 protected:
  void SetUp() override {
    table.add_column("col_1", "int", false);
    table.add_column("col_2", "string", true);
  }

  Table table{2};
};

TEST_F(StorageTableTest, ChunkCount) {
  EXPECT_EQ(table.chunk_count(), 1);
  table.append({4, "Hello,"});
  table.append({6, "world"});
  table.append({3, "!"});
  EXPECT_EQ(table.chunk_count(), 2);
}

TEST_F(StorageTableTest, GetChunk) {
  table.get_chunk(ChunkID{0});
  table.append({4, "Hello,"});
  table.append({6, "world"});
  table.append({3, "!"});
  table.get_chunk(ChunkID{1});
  const auto& reference = table;
  const auto chunk_1 = table.get_chunk(ChunkID{0});
  const auto chunk_2 = reference.get_chunk(ChunkID{1});
  EXPECT_EQ(chunk_1->size(), 2);
  EXPECT_EQ(chunk_2->size(), 1);
  EXPECT_THROW(table.get_chunk(ChunkID{7}), std::logic_error);
}

TEST_F(StorageTableTest, ColumnCount) {
  EXPECT_EQ(table.column_count(), 2);
}

TEST_F(StorageTableTest, RowCount) {
  EXPECT_EQ(table.row_count(), 0);
  table.append({4, "Hello,"});
  table.append({6, "world"});
  table.append({3, "!"});
  table.append({7, NULL_VALUE});
  EXPECT_EQ(table.row_count(), 4);
}

TEST_F(StorageTableTest, GetColumnName) {
  EXPECT_EQ(table.column_name(ColumnID{0}), "col_1");
  EXPECT_EQ(table.column_name(ColumnID{1}), "col_2");
  EXPECT_EQ(table.column_names(), std::vector<std::string>({"col_1", "col_2"}));
  EXPECT_THROW(table.column_name(ColumnID{7}), std::logic_error);
}

TEST_F(StorageTableTest, GetColumnType) {
  EXPECT_EQ(table.column_type(ColumnID{0}), "int");
  EXPECT_EQ(table.column_type(ColumnID{1}), "string");
  EXPECT_THROW(table.column_type(ColumnID{7}), std::logic_error);
}

TEST_F(StorageTableTest, ColumnNullable) {
  EXPECT_FALSE(table.column_nullable(ColumnID{0}));
  EXPECT_TRUE(table.column_nullable(ColumnID{1}));
  EXPECT_THROW(table.column_nullable(ColumnID{7}), std::logic_error);
}

TEST_F(StorageTableTest, GetColumnIdByName) {
  EXPECT_EQ(table.column_id_by_name("col_2"), 1);
  EXPECT_THROW(table.column_id_by_name("no_column_name"), std::logic_error);
}

TEST_F(StorageTableTest, GetChunkSize) {
  EXPECT_EQ(table.target_chunk_size(), 2);
}

TEST_F(StorageTableTest, AppendNullValues) {
  EXPECT_EQ(table.row_count(), 0);
  table.append({1, NULL_VALUE});
  EXPECT_EQ(table.row_count(), 1);
  EXPECT_THROW(table.append({NULL_VALUE, "foo"}), std::logic_error);
}

TEST_F(StorageTableTest, SegmentsNullable) {
  table.append({1, "foo"});
  ASSERT_EQ(table.chunk_count(), 1);
  const auto& chunk = table.get_chunk(ChunkID{0});
  ASSERT_TRUE(chunk);

  const auto& value_segment_1 = std::dynamic_pointer_cast<ValueSegment<int32_t>>(chunk->get_segment(ColumnID{0}));
  ASSERT_TRUE(value_segment_1);
  EXPECT_FALSE(value_segment_1->is_nullable());

  const auto& value_segment_2 = std::dynamic_pointer_cast<ValueSegment<std::string>>(chunk->get_segment(ColumnID{1}));
  ASSERT_TRUE(value_segment_2);
  EXPECT_TRUE(value_segment_2->is_nullable());
}

}  // namespace opossum
