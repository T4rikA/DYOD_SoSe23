#pragma once

#include "abstract_segment.hpp"

namespace opossum {

class Table;

// ReferenceSegment is a specific segment type that stores all its values as position list of a referenced column.
class ReferenceSegment : public AbstractSegment {
 public:
  // Creates a reference segment. The parameters specify the positions and the referenced column.
  ReferenceSegment(const std::shared_ptr<const Table>& referenced_table, const ColumnID referenced_column_id,
                   const std::shared_ptr<const PosList>& pos);

  AllTypeVariant operator[](const ChunkOffset chunk_offset) const override;

  ChunkOffset size() const override;

  const std::shared_ptr<const PosList>& pos_list() const;

  const std::shared_ptr<const Table>& referenced_table() const;

  ColumnID referenced_column_id() const;

  size_t estimate_memory_usage() const final;
};

}  // namespace opossum
