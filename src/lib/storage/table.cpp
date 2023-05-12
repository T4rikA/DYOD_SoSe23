#include "table.hpp"

#include "resolve_type.hpp"
#include "utils/assert.hpp"
#include "value_segment.hpp"
#include "dictionary_segment.hpp"

#include <thread>

namespace opossum {

Table::Table(const ChunkOffset target_chunk_size)
    : _chunks{}, _column_names{}, _column_types{}, _column_nullable{}, _target_chunk_size(target_chunk_size) {
  create_new_chunk();
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
}

void Table::append(const std::vector<AllTypeVariant>& values) {
  if (_chunks.back()->size() >= _target_chunk_size) {
    create_new_chunk();
  }
  _chunks.back()->append(values);
}

ColumnCount Table::column_count() const {
  return static_cast<ColumnCount>(_column_names.size());
}

uint64_t Table::row_count() const {
  return ((_chunks.size() - 1) * _target_chunk_size) + _chunks.back()->size();
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

void compress_segment(const std::shared_ptr<Chunk> old_chunk, std::shared_ptr<Chunk> new_chunk, ColumnID segment_index, std::string type){
  auto segment = old_chunk->get_segment(segment_index);
  resolve_data_type(type, [&](const auto data_type_t) {
    using ColumnDataType = typename decltype(data_type_t)::type;
    const auto dictionary_segment = std::make_shared<DictionarySegment<ColumnDataType>>(segment);
    new_chunk->add_segment_at_index(dictionary_segment, segment_index);
  });
}


// GCOVR_EXCL_START
void Table::compress_chunk(const ChunkID chunk_id) {
  // Implementation goes here
  /* You should create a new empty chunk before starting the compression, add the new dictionary-encoded segments to the
   * chunk, and in the end, put the new segments into place by exchanging the complete chunk. Keep in mind that database
   * systems are usually accessed by multiple users simultaneously. Others might access a chunk while you are
   * compressing it. Therefore, exchanging uncompressed and compressed chunks should consider concurrent accesses. */
  auto new_chunk = std::make_shared<Chunk>();
  auto old_chunk = get_chunk(chunk_id);
  auto segment_count = old_chunk->column_count();

  auto threads = std::vector<std::thread>();

  for (auto segment_index = ColumnID{}; segment_index < segment_count; ++segment_index) {
    // create thread
    auto type = this->column_type(segment_index);
    auto worker = std::thread(compress_segment, old_chunk, new_chunk, segment_index, type);
    threads.push_back(std::move(worker));
  }

  //threads join
  for (size_t thread_index = 0; thread_index < segment_count; ++thread_index) {
    threads[thread_index].join();
  }

/*  for (auto segment_index = ColumnID{}; segment_index < segment_count; ++segment_index) {
    auto segment = old_chunk->get_segment(segment_index);
    const auto& type = this->column_type(segment_index);
    resolve_data_type(type, [&](const auto data_type_t) {
      using ColumnDataType = typename decltype(data_type_t)::type;
      const auto dictionary_segment = std::make_shared<DictionarySegment<ColumnDataType>>(segment);
      new_chunk->add_segment(dictionary_segment);
    });
  }*/
  _chunks[chunk_id] = new_chunk;
}

// GCOVR_EXCL_STOP

}  // namespace opossum
