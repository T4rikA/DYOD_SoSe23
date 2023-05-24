#include "reference_segment.hpp"

#include "storage/table.hpp"
#include "utils/assert.hpp"

namespace opossum {

ReferenceSegment::ReferenceSegment(const std::shared_ptr<const Table>& referenced_table,
                                   const ColumnID referenced_column_id, const std::shared_ptr<const PosList>& pos) {
  //TODO change to more meaningful error
  Assert(referenced_table.column_name(referenced_column_id), "Table doesn't contain column_id!");
  _referenced_table = referenced_table;
  _referenced_column_id = referenced_column_id;
  _positions = pos;
}

AllTypeVariant ReferenceSegment::operator[](const ChunkOffset chunk_offset) const {
  return pos_list().at(chunk_offset);
}

ChunkOffset ReferenceSegment::size() const {
  return pos_list().size();
}

const std::shared_ptr<const PosList>& ReferenceSegment::pos_list() const {
  return _positions;
}

const std::shared_ptr<const Table>& ReferenceSegment::referenced_table() const {
  return _referenced_table;
}

ColumnID ReferenceSegment::referenced_column_id() const {
  return _referenced_column_id;
}

size_t ReferenceSegment::estimate_memory_usage() const {
  return pos_list().size()*sizeof(RowID);
}

}  // namespace opossum
