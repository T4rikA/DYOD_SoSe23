#include "table.hpp"

#include <thread>

#include "dictionary_segment.hpp"
#include "resolve_type.hpp"
#include "utils/assert.hpp"
#include "value_segment.hpp"

namespace opossum {

Table::Table(const ChunkOffset target_chunk_size)
    : _chunks{}, _column_names{}, _column_types{}, _column_nullable{}, _target_chunk_size(target_chunk_size) {
  create_new_chunk();
}

Table::Table(std::shared_ptr<Chunk> chunk, std::vector<Table::ColumnDefinitionStruct> column_definitions) {
  for (auto column_definition : column_definitions) {
    add_column_definition(column_definition.column_name, column_definition.column_type,
                          column_definition.column_nullable);
  }
  _chunks.push_back(chunk);
  // TODO(team): Do we need to set the target chunk size here?
}

void Table::add_column_definition(const std::string& name, const std::string& type, const bool nullable) {
  _column_names.emplace_back(name);
  _column_types.emplace_back(type);
  _column_nullable.emplace_back(nullable);
}

void Table::add_column(const std::string& name, const std::string& type, const bool nullable) {
  Assert(row_count() == 0, "Table is not empty, can't add column.");
  for (const auto& chunk : _chunks) {
    auto new_segment = std::shared_ptr<AbstractSegment>{};
    resolve_data_type(type, [&](auto data_type) {
      using DataType = typename decltype(data_type)::type;
      new_segment = std::make_shared<ValueSegment<DataType>>(nullable);
    });
    chunk->add_segment(new_segment);
  }
  add_column_definition(name, type, nullable);
}

void Table::create_new_chunk() {
  auto new_chunk = std::make_shared<Chunk>();
  for (auto index = uint16_t{0}; index < _column_names.size(); ++index) {
    auto new_segment = std::shared_ptr<AbstractSegment>{};
    resolve_data_type(_column_types[index], [&](auto data_type) {
      using DataType = typename decltype(data_type)::type;
      new_segment = std::make_shared<ValueSegment<DataType>>(_column_nullable[index]);
    });
    new_chunk->add_segment(new_segment);
  }
  _chunks.emplace_back(new_chunk);
  _last_chunk_encoded = false;
}

void Table::append(const std::vector<AllTypeVariant>& values) {
  if (_chunks.back()->size() >= _target_chunk_size || _last_chunk_encoded) {
    create_new_chunk();
  }
  _chunks.back()->append(values);
}

ColumnCount Table::column_count() const {
  return static_cast<ColumnCount>(_column_names.size());
}

uint64_t Table::row_count() const {
  return (chunk_count() - 1) * _chunks.front()->size() + _chunks.back()->size();
}

ChunkID Table::chunk_count() const {
  return static_cast<ChunkID>(_chunks.size());
}

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  const auto column_iterator = std::find(_column_names.begin(), _column_names.end(), column_name);
  Assert(column_iterator != _column_names.end(), "Column with name: " + column_name + " doesn't exist.");
  return static_cast<ColumnID>(std::distance(_column_names.begin(), column_iterator));
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

void compress_segment(const std::shared_ptr<AbstractSegment> segment,
                      std::vector<std::shared_ptr<AbstractSegment>>& compressed_segments, ColumnID segment_index,
                      std::string type) {
  resolve_data_type(type, [&](const auto data_type_t) {
    using ColumnDataType = typename decltype(data_type_t)::type;
    auto compressed_segment = std::make_shared<DictionarySegment<ColumnDataType>>(segment);
    compressed_segments[segment_index] = compressed_segment;
  });
}

void Table::compress_chunk(const ChunkID chunk_id) {
  const auto old_chunk = get_chunk(chunk_id);
  const auto segment_count = old_chunk->column_count();
  auto new_chunk = std::make_shared<Chunk>();
  auto compressed_segments = std::vector<std::shared_ptr<AbstractSegment>>(segment_count);

  auto threads = std::vector<std::thread>();
  for (auto segment_index = ColumnID{0}; segment_index < segment_count; ++segment_index) {
    const auto type = this->column_type(segment_index);
    const auto old_segment = old_chunk->get_segment(segment_index);
    auto worker = std::thread(compress_segment, old_segment, std::ref(compressed_segments), segment_index, type);
    threads.push_back(std::move(worker));
  }
  // threads join
  for (auto& thread : threads) {
    thread.join();
  }

  for (const auto& segment : compressed_segments) {
    new_chunk->add_segment(segment);
  }

  _chunks[chunk_id] = new_chunk;
  if (chunk_id == chunk_count() - 1) {
    _last_chunk_encoded = true;
  }
}

}  // namespace opossum
