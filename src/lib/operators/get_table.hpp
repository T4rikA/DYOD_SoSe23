#pragma once

#include "abstract_operator.hpp"
#include "utils/assert.hpp"

namespace opossum {

// Operator to retrieve a table from the StorageManager by specifying its name.
class GetTable : public AbstractOperator {
 public:
  explicit GetTable(const std::string& name) {
    // TODO(student) implement it in a source file and change this to a declaration.
  }

  const std::string& table_name() const {
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
