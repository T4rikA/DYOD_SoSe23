#include "reference_segment.hpp"

#include "storage/table.hpp"
#include "utils/assert.hpp"

namespace opossum {

ReferenceSegment::ReferenceSegment(const std::shared_ptr<const Table>& referenced_table,
                                   const ColumnID referenced_column_id, const std::shared_ptr<const PosList>& pos) {
  // Implementation goes here
}

AllTypeVariant ReferenceSegment::operator[](const ChunkOffset chunk_offset) const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

ChunkOffset ReferenceSegment::size() const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

const std::shared_ptr<const PosList>& ReferenceSegment::pos_list() const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

const std::shared_ptr<const Table>& ReferenceSegment::referenced_table() const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

ColumnID ReferenceSegment::referenced_column_id() const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

size_t ReferenceSegment::estimate_memory_usage() const {
  // Implementation goes here
  Fail("Implementation is missing.");
}

}  // namespace opossum
