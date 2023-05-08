#include "base_test.hpp"
#include "storage/storage_manager.hpp"

namespace opossum {

class StorageStorageManagerTest : public BaseTest {
 protected:
  void SetUp() override {
    auto& storage_manager = StorageManager::get();
    auto table_a = std::make_shared<Table>();
    auto table_b = std::make_shared<Table>(4);

    storage_manager.add_table("first_table", table_a);
    storage_manager.add_table("second_table", table_b);
  }
};

TEST_F(StorageStorageManagerTest, GetTable) {
  auto& storage_manager = StorageManager::get();
  auto table_c = storage_manager.get_table("first_table");
  auto table_d = storage_manager.get_table("second_table");
  EXPECT_THROW(storage_manager.get_table("third_table"), std::logic_error);
}

TEST_F(StorageStorageManagerTest, DropTable) {
  auto& storage_manager = StorageManager::get();
  storage_manager.drop_table("first_table");
  EXPECT_THROW(storage_manager.get_table("first_table"), std::logic_error);
  EXPECT_THROW(storage_manager.drop_table("first_table"), std::logic_error);
}

TEST_F(StorageStorageManagerTest, ResetTable) {
  StorageManager::get().reset();
  auto& storage_manager = StorageManager::get();
  EXPECT_THROW(storage_manager.get_table("first_table"), std::logic_error);
}

TEST_F(StorageStorageManagerTest, DoesNotHaveTable) {
  auto& storage_manager = StorageManager::get();
  EXPECT_EQ(storage_manager.has_table("third_table"), false);
}

TEST_F(StorageStorageManagerTest, HasTable) {
  auto& storage_manager = StorageManager::get();
  EXPECT_EQ(storage_manager.has_table("first_table"), true);
}

TEST_F(StorageStorageManagerTest, TableNames) {
  auto& storage_manager = StorageManager::get();
  EXPECT_EQ(storage_manager.table_names(), (std::vector<std::string>{"second_table", "first_table"}));
}

TEST_F(StorageStorageManagerTest, PrintSimpleTables) {
  auto& storage_manager = StorageManager::get();
  std::ostringstream oss;
  storage_manager.print(oss);
  EXPECT_EQ(oss.str(),
            "=== second_table ===\nn columns: 0\nn rows: 0\nn chunks: 1\ncolumns:\n"
            "=== first_table ===\nn columns: 0\nn rows: 0\nn chunks: 1\ncolumns:\n");
}

TEST_F(StorageStorageManagerTest, PrintComplexTables) {
  auto& storage_manager = StorageManager::get();
  auto table_a = storage_manager.get_table("first_table");
  table_a->add_column("first_column", "string", false);

  std::ostringstream oss;
  storage_manager.print(oss);
  EXPECT_EQ(oss.str(),
            "=== second_table ===\nn columns: 0\nn rows: 0\nn chunks: 1\ncolumns:\n"
            "=== first_table ===\nn columns: 1\nn rows: 0\nn chunks: 1\ncolumns:\n  first_column (string)\n");
}
}  // namespace opossum
