#pragma once

#include "storage/table.hpp"
#include "types.hpp"

namespace opossum {

// The StorageManager is a singleton that maintains all tables
// by mapping table names to table instances.
class StorageManager : private Noncopyable {
 public:
  static StorageManager& get();

  // Adds a table to the storage manager.
  void add_table(const std::string& name, std::shared_ptr<Table> table);

  // Removes the table from the storage manger.
  void drop_table(const std::string& name);

  // Returns the table instance with the given name.
  std::shared_ptr<Table> get_table(const std::string& name) const;

  // Returns whether the storage manager holds a table with the given name.
  bool has_table(const std::string& name) const;

  // Returns a list of all table names.
  std::vector<std::string> table_names() const;

  // Prints information about all tables in the storage manager (name, #columns, #rows, #chunks).
  void print(std::ostream& out = std::cout) const;

  // Deletes the entire StorageManager and creates a new one, used especially in tests.
  void reset();

  StorageManager(StorageManager&&) = delete;

 protected:
  StorageManager() {}

  StorageManager& operator=(StorageManager&&) = default;

  std::unordered_map<std::string, std::shared_ptr<Table>> _tables;
};

}  // namespace opossum
