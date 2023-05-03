#include "chunk.hpp"

#include "abstract_segment.hpp"
#include "utils/assert.hpp"
#include "value_segment.hpp"

namespace opossum {

void Chunk::add_segment(const std::shared_ptr<AbstractSegment> segment) {
  _segments.push_back(segment);
}

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  DebugAssert(values.size() == _segments.size(), "There must be the same amount of values as in the segment!");
  for(auto i = size_t{0}; i < values.size(); ++i){
    auto segment = _segments[i];
    {
      auto segment_pointer = dynamic_cast<ValueSegment<std::string>*>(segment.get());
      if(segment_pointer != nullptr){
        segment_pointer->append(values[i]);
      }
    }
    {
      auto segment_pointer = dynamic_cast<ValueSegment<int32_t>*>(segment.get());
      if(segment_pointer != nullptr){
        segment_pointer->append(values[i]);
      }
    }
    {
      auto segment_pointer = dynamic_cast<ValueSegment<int64_t>*>(segment.get());
      if(segment_pointer != nullptr){
        segment_pointer->append(values[i]);
      }
    }
    {
      auto segment_pointer = dynamic_cast<ValueSegment<float>*>(segment.get());
      if(segment_pointer != nullptr){
        segment_pointer->append(values[i]);
      }
    }
    {
      auto segment_pointer = dynamic_cast<ValueSegment<double>*>(segment.get());
      if(segment_pointer != nullptr){
        segment_pointer->append(values[i]);
      }
    }
  }
}

std::shared_ptr<AbstractSegment> Chunk::get_segment(const ColumnID column_id) const {
  return _segments[column_id];
}

ColumnCount Chunk::column_count() const {
  return ColumnCount(_segments.size());
}

ChunkOffset Chunk::size() const {
  if (_segments.size()) {
    return _segments[0]->size();
  }
  return 0;
}

}  // namespace opossum
