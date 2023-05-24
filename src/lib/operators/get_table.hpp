#pragma once

#include "abstract_operator.hpp"
#include "utils/assert.hpp"

namespace opossum {

// Operator to retrieve a table from the StorageManager by specifying its name.
class GetTable : public AbstractOperator {
 public:
  explicit GetTable(const std::string& name);

  // Returns the table name
  const std::string& table_name() const;

 protected:
  std::shared_ptr<const Table> _on_execute() override;

  std::shared_ptr<Table> _table;
  std::string _table_name;
};

}  // namespace opossum
