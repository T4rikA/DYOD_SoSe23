#pragma once

#include "abstract_operator.hpp"
#include "utils/assert.hpp"
#include "all_type_variant.hpp"

namespace opossum {

class TableScan : public AbstractOperator {
 public:
  TableScan(const std::shared_ptr<const AbstractOperator>& in, const ColumnID column_id, const ScanType scan_type,
            const AllTypeVariant search_value);

  ColumnID column_id() const;

  ScanType scan_type() const;

  const AllTypeVariant& search_value() const;

 protected:
  std::shared_ptr<const Table> _on_execute() override {
    // TODO(student) implement it in a source file and change this to a declaration.
    Fail("Implementation missing.");
  }
};

}  // namespace opossum
