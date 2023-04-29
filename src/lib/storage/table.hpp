#pragma once

#include "abstract_segment.hpp"
#include "chunk.hpp"
#include "type_cast.hpp"

namespace opossum {

class TableStatistics;

// A table is partitioned horizontally into a number of chunks
class Table : private Noncopyable {
 public:
  // Creates a table. The parameter specifies the maximum chunk size, i.e., partition size default is the maximum chunk
  // size minus 1. A table always holds at least one chunk.
  explicit Table(const ChunkOffset target_chunk_size = std::numeric_limits<ChunkOffset>::max() - 1);

  // Returns the number of columns (cannot exceed ColumnID (uint16_t)).
  ColumnCount column_count() const;

  // Returns the number of rows. This number includes invalidated (deleted) rows. Use approx_valid_row_count() for an
  // approximate count of valid rows instead.
  uint64_t row_count() const;

  // Returns the number of chunks (cannot exceed ChunkID (uint32_t)).
  ChunkID chunk_count() const;

  // Returns the chunk with the given id.
  std::shared_ptr<Chunk> get_chunk(const ChunkID chunk_id);
  std::shared_ptr<const Chunk> get_chunk(const ChunkID chunk_id) const;

  // Returns a list of all column names.
  const std::vector<std::string>& column_names() const;

  // Returns the column name of the nth column.
  const std::string& column_name(const ColumnID column_id) const;

  // Returns the column type of the nth column.
  const std::string& column_type(const ColumnID column_id) const;

  // Returns whether the nth column can contain NULL values.
  bool column_nullable(const ColumnID column_id) const;

  // Returns the column with the given name. This method is intended for debugging purposes only. It does not verify
  // whether a column name is unambiguous.
  ColumnID column_id_by_name(const std::string& column_name) const;

  // Return the target chunk size (cannot exceed ChunkOffset (uint32_t)).
  ChunkOffset target_chunk_size() const;

  // Adds column definition without creating the actual columns. This is helpful when, e.g., an operator first creates
  // the structure of the table and then adds chunk by chunk.
  void add_column_definition(const std::string& name, const std::string& type, const bool nullable);

  // Adds a column to the end, i.e., right, of the table. This can only be done if the table does not yet have any
  // entries, because we would otherwise have to deal with default values.
  void add_column(const std::string& name, const std::string& type, const bool nullable);

  // Inserts a row at the end of the table. Note this is slow and not thread-safe and should be used for testing
  // purposes only.
  void append(const std::vector<AllTypeVariant>& values);

  // Creates a new chunk and appends it.
  void create_new_chunk();

  // Compresses a ValueColumn into a DictionaryColumn.
  void compress_chunk(const ChunkID chunk_id);

 protected:
  std::vector<std::shared_ptr<Chunk>> _chunks;
  std::vector<std::string> _column_names;
  std::vector<std::string> _column_types;
  std::vector<bool> _column_nullable;
  int32_t _column_count = 0;
  int32_t _chunk_count = 0;
  ChunkOffset _target_chunk_size;
};

}  // namespace opossum
