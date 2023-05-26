#pragma once

#include "abstract_operator.hpp"
#include "utils/assert.hpp"

namespace opossum {

class TableScan : public AbstractOperator {
 public:
  TableScan(const std::shared_ptr<const AbstractOperator>& in, const ColumnID column_id, const ScanType scan_type,
            const AllTypeVariant search_value) {
    // TODO(student) implement it in a source file and change this to a declaration.
  }

  ColumnID column_id() const {
    // TODO(student) implement it in a source file and change this to a declaration.
    Fail("Implementation missing.");
  }

  ScanType scan_type() const {
    // TODO(student) implement it in a source file and change this to a declaration.
    Fail("Implementation missing.");
  }

  const AllTypeVariant& search_value() const {
    // TODO(student) implement it in a source file and change this to a declaration.
    Fail("Implementation missing.");
  }

 protected:
  std::shared_ptr<const Table> _on_execute() override {
    // TODO(student) implement it in a source file and change this to a declaration.
    Fail("Implementation missing.");
  }
};

}  // namespace opossum
