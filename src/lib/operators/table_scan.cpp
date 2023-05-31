#include "table_scan.hpp"
#include "all_type_variant.hpp"
#include "resolve_type.hpp"
#include "storage/abstract_attribute_vector.hpp"
#include "storage/dictionary_segment.hpp"
#include "storage/reference_segment.hpp"
#include "storage/table.hpp"
#include "storage/value_segment.hpp"

namespace opossum {

TableScan::TableScan(const std::shared_ptr<const AbstractOperator>& in, const ColumnID column_id,
                     const ScanType scan_type, const AllTypeVariant search_value) {
  _in = in;
  _column_id = column_id;
  _scan_type = scan_type;
  _search_value = search_value;
}

ColumnID TableScan::column_id() const {
  return _column_id;
}

ScanType TableScan::scan_type() const {
  return _scan_type;
}

const AllTypeVariant& TableScan::search_value() const {
  return _search_value;
}

template <typename T>
bool TableScan::compare(const T& a, const T& b) {
  switch (scan_type()) {
    case ScanType::OpEquals: {
      return a == b;
    }
    case ScanType::OpNotEquals: {
      return a != b;
    }
    case ScanType::OpLessThan: {
      return a < b;
    }
    case ScanType::OpLessThanEquals: {
      return a <= b;
    }
    case ScanType::OpGreaterThan: {
      return a > b;
    }
    case ScanType::OpGreaterThanEquals: {
      return a >= b;
    }
  }
  Assert(true, "Scan uses an invalid Scan Type.");
  return false;
}

template <typename T>
void TableScan::scan_value_segment(const std::shared_ptr<ValueSegment<T>>& segment, PosList& positions_list,
                                   ChunkID chunk_id) {
  auto values = segment->values();
  auto value_count = segment->size();
  const auto typed_search_value = type_cast<T>(search_value());

  for (auto chunk_offset = ChunkOffset{0}; chunk_offset < value_count; ++chunk_offset) {
    if (segment->is_null(chunk_offset)) {
      continue;
    }

    const auto value = type_cast<T>(values[chunk_offset]);
    if (compare(value, typed_search_value)) {
      positions_list.push_back({RowID{chunk_id, chunk_offset}});
    }
  }
}

// TODO(team): Check if type cast works.
template <typename T>
void TableScan::scan_dict_segment(const std::shared_ptr<DictionarySegment<T>>& segment, PosList& positions_list,
                                  ChunkID chunk_id) {
  auto dictionary = segment->dictionary();
  auto attribute_vector = segment->attribute_vector();
  auto value_count = segment->size();

  const auto typed_search_value = type_cast<T>(search_value());

  for (auto chunk_offset = ChunkOffset{0}; chunk_offset < value_count; ++chunk_offset) {
    if (variant_is_null(segment->operator[](chunk_offset))) {
      continue;
    }

    const auto value = segment->get(chunk_offset);
    if (compare(value, typed_search_value)) {
      positions_list.push_back(RowID{chunk_id, chunk_offset});
    }
  }
}

void TableScan::scan_reference_segment(const std::shared_ptr<ReferenceSegment> segment, PosList& positions_list) {
  const auto typed_search_value = search_value();

  for (const auto row_id : *segment->pos_list()) {
    const auto value = segment->get_row_id(row_id);
    if (variant_is_null(value)) {
      continue;
    }

    if (compare(value, typed_search_value)) {
      positions_list.push_back(row_id);
    }
  }
}

std::shared_ptr<const Table> TableScan::_on_execute() {
  // get posListPtr, tableptr, columncount, data type of table
  auto table = _in->get_output();
  auto column_count = table->column_count();
  auto data_type = table->column_type(column_id());
  auto chunk_count = table->chunk_count();
  auto positions_list = std::make_shared<PosList>();

  Assert(!variant_is_null(search_value()), "Table Scan Error: Can not compare against NULL value");

  // Iterate over all Chunks and decide how to execute the scan depending on the type of segment
  for (auto chunk_id = ChunkID{0}; chunk_id < chunk_count; chunk_id++) {
    auto segment = table->get_chunk(chunk_id)->get_segment(column_id());

    resolve_data_type(data_type, [&segment, this, &positions_list, &chunk_id, &table](auto type) {
      using Type = typename decltype(type)::type;

      const auto typed_value_segment = std::dynamic_pointer_cast<ValueSegment<Type>>(segment);
      const auto typed_dict_segment = std::dynamic_pointer_cast<DictionarySegment<Type>>(segment);
      const auto reference_segment = std::dynamic_pointer_cast<ReferenceSegment>(segment);

      Assert(typed_value_segment || typed_dict_segment || reference_segment,
             "TableScan error: segment is not from value segment, dict segment or reference segment.");
      if (typed_value_segment) {
        scan_value_segment(typed_value_segment, *positions_list, chunk_id);
      } else if (typed_dict_segment) {
        scan_dict_segment(typed_dict_segment, *positions_list, chunk_id);
      } else {
        scan_reference_segment(reference_segment, *positions_list);
        table = reference_segment->referenced_table();
      }
    });
  }

  // create ref segments and add them
  auto chunk = std::make_shared<Chunk>();
  for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {
    auto reference_segment = std::make_shared<ReferenceSegment>(table, column_id, positions_list);
    chunk->add_segment(reference_segment);
  }

  auto column_definitions = std::vector<Table::ColumnDefinitionStruct>();
  for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {
    auto column_definition = Table::ColumnDefinitionStruct{table->column_name(column_id), table->column_type(column_id),
                                                           table->column_nullable(column_id)};
    column_definitions.push_back(column_definition);
  }

  // build the output table
  auto output_table = std::make_shared<Table>(chunk, column_definitions);

  return output_table;
}
}  // namespace opossum
