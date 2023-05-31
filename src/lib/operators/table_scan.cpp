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
std::shared_ptr<PosList> TableScan::scan_value_segment(const ValueSegment<T>& segment, const std::shared_ptr<PosList>& positions_list, const ChunkID chunk_id) {
  auto values = segment.values();
  auto value_count = segment.size();

  // TODO: Refactor into function
  for (auto chunk_offset = ChunkOffset{0}; chunk_offset < value_count; ++chunk_offset) {
    auto value = values[chunk_offset];
    const auto typed_search_value = type_cast<T>(search_value());
    const auto typed_given_value = type_cast<T>(value);

    switch (scan_type()) {
      case ScanType::OpEquals:
        if (typed_given_value == typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpNotEquals:
        if (typed_given_value != typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpLessThan:
        if (typed_given_value < typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpLessThanEquals:
        if (typed_given_value <= typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpGreaterThan:
        if (typed_given_value > typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpGreaterThanEquals:
        if (typed_given_value >= typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
    }
  };

  return positions_list;
}

// TODO check type cast works
template <typename T>
std::shared_ptr<PosList> TableScan::scan_dict_segment(const DictionarySegment<T>& segment, const std::shared_ptr<PosList>& positions_list, const ChunkID chunk_id) {
  auto dictionary = segment->dictionary();
  auto attribute_vector = segment->attribute_vector();
  auto value_count = segment->size();
  const auto typed_search_value = type_cast<T>(search_value());

  // TODO: Refactor into function
  for (auto chunk_offset = ChunkOffset{0}; chunk_offset < value_count; ++chunk_offset) {
    auto value = dictionary[attribute_vector->get(chunk_offset)];
    const auto typed_given_value = type_cast<T>(value);

    switch (scan_type()) {
      case ScanType::OpEquals:
        if (typed_given_value == typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpNotEquals:
        if (typed_given_value != typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpLessThan:
        if (typed_given_value < typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpLessThanEquals:
        if (typed_given_value <= typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpGreaterThan:
        if (typed_given_value > typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
      case ScanType::OpGreaterThanEquals:
        if (typed_given_value >= typed_search_value) {
          positions_list->push_back(RowID{chunk_id, chunk_offset});
        }
        break;
    }
  }

  return positions_list;
}

std::shared_ptr<PosList> TableScan::scan_reference_segment(const ReferenceSegment& segment, const std::shared_ptr<PosList>& positions_list) {

  return positions_list;
}

std::shared_ptr<const Table> TableScan::_on_execute() {
  // get posListPtr, tableptr, columncount, data type of table
  auto table = _in->get_output();
  auto column_count = table->column_count();
  auto data_type = table->column_type(column_id());
  auto chunk_count = table->chunk_count();
  auto positions_list = std::make_shared<PosList>;

  // go over all segments of table and depending on their type, collect the pos lists of the matching values to search value
  for (auto chunk_id = ChunkID{0}; chunk_id < chunk_count; chunk_id++) {
    auto segment = table->get_chunk(chunk_id)->get_segment(column_id());

    resolve_data_type(data_type, [&segment, this, &positions_list, &chunk_id](auto type) {
      using Type = typename decltype(type)::type;

      auto typed_value_segment = std::dynamic_pointer_cast<ValueSegment<Type>>(segment);
      auto typed_dict_segment = std::dynamic_pointer_cast<DictionarySegment<Type>>(segment);
      auto reference_segment = std::dynamic_pointer_cast<ReferenceSegment>(segment);

      Assert(typed_value_segment || typed_dict_segment || reference_segment,
             "TableScan error: segment is not from value segment, dict segment or reference segment.");
      if (typed_value_segment) {
        scan_value_segment(*typed_value_segment, positions_list, chunk_id);
      } else if (typed_dict_segment) {
        scan_dict_segment(*typed_dict_segment, positions_list, chunk_id);
      } else {
        scan_reference_segment(*reference_segment, positions_list);
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