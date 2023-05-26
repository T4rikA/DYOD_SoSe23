#include "table_wrapper.hpp"

namespace opossum {

TableWrapper::TableWrapper(const std::shared_ptr<const Table>& table) : _table(table) {}

std::shared_ptr<const Table> TableWrapper::_on_execute() {
  return _table;
}
}  // namespace opossum
