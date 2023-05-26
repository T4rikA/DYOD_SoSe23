#include "table_scan.hpp"
#include "all_type_variant.hpp"

namespace opossum {

TableScan::TableScan(const std::shared_ptr<const AbstractOperator>& in, const ColumnID column_id, const ScanType scan_type,
          const AllTypeVariant search_value) {}


ColumnID TableScan::column_id() const {
  Fail("Implementation missing.");
}

ScanType TableScan::scan_type() const {
  Fail("Implementation missing.");
}

const AllTypeVariant& TableScan::search_value() const {
  Fail("Implementation missing.");
}



}  // namespace opossum