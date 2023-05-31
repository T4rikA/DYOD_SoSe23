#pragma once

#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "storage/table.hpp"
#include "utils/assert.hpp"
#include "storage/dictionary_segment.hpp"
#include "storage/reference_segment.hpp"
#include "storage/value_segment.hpp"


namespace opossum {

class TableScan : public AbstractOperator {
 public:
  // TODO: Add comments
  TableScan(const std::shared_ptr<const AbstractOperator>& in, const ColumnID column_id, const ScanType scan_type,
            const AllTypeVariant search_value);
  ColumnID column_id() const;
  ScanType scan_type() const;
  const AllTypeVariant& search_value() const;

 protected:
  std::shared_ptr<const Table> _on_execute();
  template <typename T>
  std::shared_ptr<PosList> scan_value_segment(const ValueSegment<T>& segment, const std::shared_ptr<PosList>& positions_list, ChunkID chunk_id);
  template <typename T>
  std::shared_ptr<PosList> scan_dict_segment(const DictionarySegment<T>& segment, const std::shared_ptr<PosList>& positions_list, ChunkID chunk_id);
  std::shared_ptr<PosList> scan_reference_segment(const ReferenceSegment& segment, const std::shared_ptr<PosList>& positions_list);
  std::shared_ptr<const AbstractOperator> _in;
  ColumnID _column_id;
  ScanType _scan_type;
  AllTypeVariant _search_value;
};

}  // namespace opossum
