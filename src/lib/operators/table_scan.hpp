#pragma once

#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "storage/dictionary_segment.hpp"
#include "storage/reference_segment.hpp"
#include "storage/table.hpp"
#include "storage/value_segment.hpp"
#include "utils/assert.hpp"

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
  void scan_value_segment(const std::shared_ptr<ValueSegment<T>>& segment, PosList& positions_list,
                                              ChunkID chunk_id);
  template <typename T>
  std::shared_ptr<PosList> scan_dict_segment(const std::shared_ptr<DictionarySegment<T>>& segment,
                                             PosList& positions_list, ChunkID chunk_id);
  std::shared_ptr<PosList> scan_reference_segment(const std::shared_ptr<ReferenceSegment> segment,
                                                  PosList& positions_list);
  std::shared_ptr<const AbstractOperator> _in;
  ColumnID _column_id;
  ScanType _scan_type;
  AllTypeVariant _search_value;
};

}  // namespace opossum
