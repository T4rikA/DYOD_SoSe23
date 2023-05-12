#include "chunk.hpp"

#include "abstract_segment.hpp"
#include "resolve_type.hpp"
#include "utils/assert.hpp"
#include "value_segment.hpp"

namespace opossum {

void Chunk::add_segment(const std::shared_ptr<AbstractSegment> segment) {
  _segments.push_back(segment);
}

void Chunk::add_segment_at_index(const std::shared_ptr<AbstractSegment> segment, ColumnID index) {
  Assert(index < column_count(), "Index too big.");
  _segments[index] = segment;
}

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  static const auto data_types = std::vector<std::string>{"int", "long", "float", "double", "string"};
  const auto column_count = _segments.size();
  Assert(values.size() == column_count, "Number of segments does not match value list.");

  for (auto column_id = ColumnID{0}; column_id < column_count; ++column_id) {
    auto success = false;
    for (const auto& data_type : data_types) {
      if (success) {
        break;
      }
      resolve_data_type(data_type, [&](auto type) {
        using DataType = typename decltype(type)::type;
        if (const auto& typed_segment = std::dynamic_pointer_cast<ValueSegment<DataType>>(_segments[column_id])) {
          typed_segment->append(values[column_id]);
          success = true;
        }
      });
    }
  }
}

std::shared_ptr<AbstractSegment> Chunk::get_segment(const ColumnID column_id) const {
  return _segments.at(column_id);
}

ColumnCount Chunk::column_count() const {
  return ColumnCount(_segments.size());
}

ChunkOffset Chunk::size() const {
  if (!column_count()) {
    return 0;
  }
  return _segments[0]->size();
}

}  // namespace opossum
