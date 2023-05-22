#include "print.hpp"

#include <iomanip>

#include "operators/table_wrapper.hpp"
#include "storage/abstract_segment.hpp"
#include "storage/table.hpp"
#include "type_cast.hpp"

namespace {

using namespace opossum;  // NOLINT(build/namespaces)

std::string print_column_type(const std::shared_ptr<const Table>& table, const ColumnID column_id) {
  auto stream = std::stringstream{};
  stream << table->column_type(column_id);
  if (table->column_nullable(column_id)) {
    stream << "_null";
  }

  return stream.str();
}

}  // namespace

namespace opossum {

Print::Print(const std::shared_ptr<const AbstractOperator> in, std::ostream& out) : AbstractOperator(in), _out(out) {}

void Print::print(std::shared_ptr<const Table>& table, std::ostream& out) {
  auto table_wrapper = std::make_shared<TableWrapper>(table);
  table_wrapper->execute();
  Print(table_wrapper, out).execute();
}

std::shared_ptr<const Table> Print::_on_execute() {
  auto widths = _column_string_widths(8, 20, _left_input_table());

  // Print column headers.
  _out << "=== Columns" << std::endl;
  const auto left_column_count = _left_input_table()->column_count();
  for (auto column_id = ColumnID{0}; column_id < left_column_count; ++column_id) {
    _out << "|" << std::setw(widths[column_id]) << _left_input_table()->column_name(column_id) << std::setw(0);
  }
  _out << "|" << std::endl;
  for (auto column_id = ColumnID{0}; column_id < left_column_count; ++column_id) {
    _out << "|" << std::setw(widths[column_id]) << print_column_type(_left_input_table(), column_id) << std::setw(0);
  }
  _out << "|" << std::endl;

  // print each chunk
  const auto left_chunk_count = _left_input_table()->chunk_count();
  for (auto chunk_id = ChunkID{0}; chunk_id < left_chunk_count; ++chunk_id) {
    const auto chunk = _left_input_table()->get_chunk(chunk_id);

    _out << "=== Chunk " << chunk_id << " === " << std::endl;

    if (chunk->size() == 0) {
      _out << "Empty chunk." << std::endl;
      continue;
    }

    // Print the rows in the chunk.
    const auto chunk_size = chunk->size();
    for (size_t row = 0; row < chunk_size; ++row) {
      _out << "|";
      const auto column_count = chunk->column_count();
      for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {
        // Yes, we use AbstractSegment::operator[] here, but since Print is not an operation that should be part of a
        // regular query plan, let's keep things simple here.
        _out << std::setw(widths[column_id]) << (*chunk->get_segment(column_id))[row] << "|" << std::setw(0);
      }

      _out << std::endl;
    }
  }

  return _left_input_table();
}

// In order to print the table as an actual table, with columns being aligned, we need to calculate the number of
// characters in the printed representation of each column `min` and `max` can be used to limit the width of the
// columns - however, every column fits at least the column's name.
std::vector<uint16_t> Print::_column_string_widths(uint16_t min, uint16_t max,
                                                   const std::shared_ptr<const Table>& table) const {
  auto widths = std::vector<uint16_t>(table->column_count());
  // Calculate the length of the column name.
  const auto column_count = table->column_count();
  for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {
    widths[column_id] = std::max(min, static_cast<uint16_t>(table->column_name(column_id).size()));
  }

  // Go over all rows and find the maximum length of the printed representation of a value, up to max.
  const auto left_chunk_count = _left_input_table()->chunk_count();
  for (auto chunk_id = ChunkID{0}; chunk_id < left_chunk_count; ++chunk_id) {
    auto chunk = _left_input_table()->get_chunk(chunk_id);

    const auto column_count = chunk->column_count();
    for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {
      for (auto row = size_t{0}; row < chunk->size(); ++row) {
        auto cell_length =
            static_cast<uint16_t>(boost::lexical_cast<std::string>((*chunk->get_segment(column_id))[row]).size());
        widths[column_id] = std::max({min, widths[column_id], std::min(max, cell_length)});
      }
    }
  }
  return widths;
}

}  // namespace opossum
