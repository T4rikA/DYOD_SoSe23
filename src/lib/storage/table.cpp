#include "table.hpp"

#include "utils/assert.hpp"
#include "value_segment.hpp"

namespace opossum {

Table::Table(const ChunkOffset target_chunk_size) {
  _target_chunk_size = target_chunk_size;
  _chunks = std::vector<std::shared_ptr<Chunk>>();
  _column_names = std::vector<std::string>();
  _column_types = std::vector<std::string>();
  _column_nullable = std::vector<bool>();
  create_new_chunk();
}

void Table::add_column_definition(const std::string& name, const std::string& type, const bool nullable) {
  _column_names.emplace_back(name);
  _column_types.emplace_back(type);
  _column_nullable.emplace_back(nullable);
}

void Table::add_column(const std::string& name, const std::string& type, const bool nullable) {
  for (auto chunk : _chunks) {
    auto new_segment = std::shared_ptr<AbstractSegment>{};
    if (type == "string") {
      new_segment = std::make_shared<ValueSegment<std::string>>(nullable);
    } else {
      new_segment = std::make_shared<ValueSegment<int32_t>>(nullable);
    }
    chunk->add_segment(new_segment);
  }
  add_column_definition(name, type, nullable);
  _column_count++;
}

void Table::create_new_chunk() {
  auto new_chunk = std::make_shared<Chunk>();
  for (int index = 0; index < _column_count; ++index) {
    auto new_segment = std::shared_ptr<AbstractSegment>{};
    if (_column_types.at(index) == "string") {
      new_segment = std::make_shared<ValueSegment<std::string>>(_column_nullable.at(index));
    } else {
      new_segment = std::make_shared<ValueSegment<int32_t>>(_column_nullable.at(index));
    }
    new_chunk->add_segment(new_segment);
  }
  _chunks.emplace_back(new_chunk);
  _chunk_count++;
}

void Table::append(const std::vector<AllTypeVariant>& values) {
  if (_chunks.back()->size() < _target_chunk_size) {
    _chunks.back()->append(values);
  } else {
    create_new_chunk();
    _chunks.back()->append(values);
  }
}

ColumnCount Table::column_count() const {
  return ColumnCount{_column_count};
}

uint64_t Table::row_count() const {
  return ((_chunk_count - 1) * _target_chunk_size) + _chunks.back()->size();
}

ChunkID Table::chunk_count() const {
  return ChunkID{_chunk_count};
}

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  for (auto index = uint16_t{0}; index < _column_count; ++index) {
    if (_column_names.at(index) == column_name) {
      return ColumnID{index};
    }
  }
  throw std::logic_error("Name does not exist");
}

ChunkOffset Table::target_chunk_size() const {
  return _target_chunk_size;
}

const std::vector<std::string>& Table::column_names() const {
  return _column_names;
}

const std::string& Table::column_name(const ColumnID column_id) const {
  return _column_names.at(column_id);
}

const std::string& Table::column_type(const ColumnID column_id) const {
  return _column_types.at(column_id);
}

bool Table::column_nullable(const ColumnID column_id) const {
  return _column_nullable.at(column_id);
}

std::shared_ptr<Chunk> Table::get_chunk(ChunkID chunk_id) {
  return _chunks.at(chunk_id);
}

std::shared_ptr<const Chunk> Table::get_chunk(ChunkID chunk_id) const {
  return _chunks.at(chunk_id);
}

// GCOVR_EXCL_START
void Table::compress_chunk(const ChunkID chunk_id) {
  // Implementation goes here
  Fail("Implementation is missing.");
}

// GCOVR_EXCL_STOP

}  // namespace opossum
