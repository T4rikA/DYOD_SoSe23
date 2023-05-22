#include "base_test.hpp"

#include "operators/get_table.hpp"
#include "operators/print.hpp"
#include "storage/storage_manager.hpp"
#include "storage/table.hpp"

namespace opossum {

class OperatorsPrintTest : public BaseTest {
 protected:
  void SetUp() override {
    _table = std::make_shared<Table>(_chunk_size);
    _table->add_column("col_1", "int", false);
    _table->add_column("col_2", "string", true);
    StorageManager::get().add_table(_table_name, _table);

    _get_table_oper = std::make_shared<GetTable>(_table_name);
    _get_table_oper->execute();
  }

  std::ostringstream _output;
  std::string _table_name = "printTestTable";
  uint32_t _chunk_size = 10;
  std::shared_ptr<GetTable> _get_table_oper;
  std::shared_ptr<Table> _table = nullptr;
};

// Class used to make protected methods visible without modifying the base class with testing code.
class PrintWrapper : public Print {
  std::shared_ptr<const Table> _table;

 public:
  explicit PrintWrapper(const std::shared_ptr<AbstractOperator> in) : Print(in), _table(in->get_output()) {}

  std::vector<uint16_t> test_column_string_widths(uint16_t min, uint16_t max) {
    return _column_string_widths(min, max, _table);
  }
};

TEST_F(OperatorsPrintTest, EmptyTable) {
  auto print = std::make_shared<Print>(_get_table_oper, _output);
  print->execute();

  // check if table is correctly passed
  EXPECT_EQ(print->get_output(), _table);

  auto output_str = _output.str();

  // rather hard-coded tests
  EXPECT_TRUE(output_str.find("col_1") != std::string::npos);
  EXPECT_TRUE(output_str.find("col_2") != std::string::npos);
  EXPECT_TRUE(output_str.find("int") != std::string::npos);
  EXPECT_TRUE(output_str.find("string_null") != std::string::npos);
  EXPECT_TRUE(output_str.find("Empty chunk.") != std::string::npos);
}

TEST_F(OperatorsPrintTest, FilledTable) {
  auto table = StorageManager::get().get_table(_table_name);
  for (auto index = size_t{0}; index < _chunk_size * 2; ++index) {
    // Char 97 is an 'a'
    table->append(
        {static_cast<int32_t>(index % _chunk_size), std::string(1, 97 + static_cast<int32_t>(index / _chunk_size))});
  }

  auto print = std::make_shared<Print>(_get_table_oper, _output);
  print->execute();

  // Check if table is correctly passed.
  EXPECT_EQ(print->get_output(), table);

  auto output_str = _output.str();

  EXPECT_TRUE(output_str.find("Chunk 0") != std::string::npos);
  // There should not be a third chunk (at least that's the current impl).
  EXPECT_TRUE(output_str.find("Chunk 3") == std::string::npos);

  // Remove spaces.
  output_str.erase(remove_if(output_str.begin(), output_str.end(), isspace), output_str.end());

  EXPECT_TRUE(output_str.find("|2|a|") != std::string::npos);
  EXPECT_TRUE(output_str.find("|9|b|") != std::string::npos);
  EXPECT_TRUE(output_str.find("|10|a|") == std::string::npos);
}

TEST_F(OperatorsPrintTest, GetColumnWidths) {
  auto min = size_t{8};
  auto max = size_t{20};

  auto table = StorageManager::get().get_table(_table_name);

  auto print_wrapper = std::make_shared<PrintWrapper>(_get_table_oper);
  auto print_lengths = print_wrapper->test_column_string_widths(min, max);

  // We have two columns, thus two 'lengths'.
  ASSERT_EQ(print_lengths.size(), size_t{2});
  // With empty columns and short column names, we should see the minimal lengths.
  EXPECT_EQ(print_lengths.at(0), min);
  EXPECT_EQ(print_lengths.at(1), min);

  int ten_digits_ints = 1234567890;

  table->append({ten_digits_ints, "quite a long string with more than $max chars"});

  print_lengths = print_wrapper->test_column_string_widths(min, max);
  EXPECT_EQ(print_lengths.at(0), size_t{10});
  EXPECT_EQ(print_lengths.at(1), max);
}

}  // namespace opossum
