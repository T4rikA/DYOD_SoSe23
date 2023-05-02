#include "storage_manager.hpp"

#include "utils/assert.hpp"

namespace opossum {

StorageManager& StorageManager::get() {
  static auto instance = StorageManager{};
  return instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) {
  _tables[name] = table;
}

void StorageManager::drop_table(const std::string& name) {
  if (_tables[name]) {
    _tables[name] = nullptr;
  } else {
    throw std::logic_error("Table does not exist");
  }
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  if (_tables.at(name)) {
    return _tables.at(name);
  } else {
    throw std::logic_error("Table does not exist");
  }
}

bool StorageManager::has_table(const std::string& name) const {
  if (_tables.contains(name))
    return true;
  return false;
}

std::vector<std::string> StorageManager::table_names() const {
  auto keys = std::vector<std::string>{};
  keys.reserve(_tables.size());

  for (auto [key, _] : _tables) {
    keys.push_back(key);
  }

  return keys;
}

void StorageManager::print(std::ostream& out) const {
  for (auto const& [table_name, table] : _tables) {
    out << "=== " << table_name << " ===" << std::endl;
    out << "n columns: " << table->column_count() << std::endl;
    out << "n rows: " << table->row_count() << std::endl;
    out << "n chunks: " << table->chunk_count() << std::endl;
    out << "columns:" << std::endl;
    for (auto column_id = ColumnID{0}; column_id < table->column_count(); column_id++) {
      out << "  " << table->column_name(column_id) << " (" << table->column_type(column_id) << ")" << std::endl;
    }
  }
}

void StorageManager::reset() {
  _tables = std::unordered_map<std::string, std::shared_ptr<Table>>();
}

}  // namespace opossum
