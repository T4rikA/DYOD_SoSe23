#pragma once

#include "abstract_operator.hpp"

namespace opossum {

/**
 * operator to print the table with its data
 */
class Print : public AbstractOperator {
 public:
  explicit Print(const std::shared_ptr<const AbstractOperator> in, std::ostream& out = std::cout);

  static void print(std::shared_ptr<const Table>& table, std::ostream& out = std::cout);

 protected:
  std::vector<uint16_t> _column_string_widths(uint16_t min, uint16_t max,
                                              const std::shared_ptr<const Table>& table) const;
  std::shared_ptr<const Table> _on_execute() override;

  // stream to print the result
  std::ostream& _out;
};
}  // namespace opossum
