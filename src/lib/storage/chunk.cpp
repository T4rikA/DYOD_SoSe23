#include "chunk.hpp"

#include "abstract_segment.hpp"
#include "utils/assert.hpp"

namespace opossum {

void Chunk::add_segment(const std::shared_ptr<AbstractSegment> segment) {
  segments.push_back(segment);
}

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  DebugAssert(values.size() == segments.size(), "There must be the same amount of values as in the segment!");
  for(size_t i = 0; i < values.size(); ++i){
    auto segment = segments[i];
    segment->append(values[i]);
  }
}

std::shared_ptr<AbstractSegment> Chunk::get_segment(const ColumnID column_id) const {
  return segments[column_id];
}

ColumnCount Chunk::column_count() const {
  return ColumnCount(segments.size()); 
}

ChunkOffset Chunk::size() const {
  if(segments.size()){
    return segments[0] -> size();
  }
  return 0;
}

}  // namespace opossum
