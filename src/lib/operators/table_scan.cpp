#include "table_scan.hpp"
#include "all_type_variant.hpp"
#include "resolve_type.hpp"
#include "storage/dictionary_segment.hpp"
#include "storage/reference_segment.hpp"
#include "storage/value_segment.hpp"

namespace opossum {

TableScan::TableScan(const std::shared_ptr<const AbstractOperator>& in, const ColumnID column_id, const ScanType scan_type,
          const AllTypeVariant search_value) {
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

std::shared_ptr<const Table> TableScan::_on_execute() {
  // get posListPtr, tableptr, columncount, data type of table
  auto positions_pointers = std::make_shared<PosList>();
  auto table = _left_input_table();
  auto column_count = table->column_count();
  auto data_type = table->column_type(_column_id);
  auto chunk_count = table->chunk_count();
  // go over all segments of table and depending on their type, collect the pos lists of the matching values to search value
  for(auto chunk_id = ChunkID{0}; chunk_id<chunk_count; chunk_id++){
    auto segment = table->get_chunk(chunk_id)->get_segment(_column_id);
    resolve_data_type(data_type, [&] (auto type) {
      using Type = typename decltype(type)::type;

      auto typed_value_segment = std::dynamic_pointer_cast<ValueSegment<Type>>(segment);
      if(typed_value_segment){
        // scan value segment
      }

      auto typed_dict_segment = std::dynamic_pointer_cast<DictionarySegment<Type>>(segment);
      if(typed_dict_segment){
        // scan dict segment
      }

    });
  }
  // build the output table
  auto output_table = std::make_shared<Table>();
  // - create column defs first
  for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {
    output_table->add_column(table->column_name(column_id), table->column_type(column_id), table->column_nullable(column_id));
  }
  // - create ref segments and add them
  auto chunk = std::make_shared<Chunk>();
  for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {

    auto reference_segment = std::make_shared<ReferenceSegment>(table, column_id, positions_pointers);
    chunk->add_segment(reference_segment);
  }
  return output_table;
}
}  // namespace opossum