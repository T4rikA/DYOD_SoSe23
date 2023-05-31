#include "abstract_operator.hpp"
#include "utils/assert.hpp"

namespace opossum {

AbstractOperator::AbstractOperator(const std::shared_ptr<const AbstractOperator> left,
                                   const std::shared_ptr<const AbstractOperator> right)
    : _left_input(left), _right_input(right) {}

void AbstractOperator::execute() {
  _output = _on_execute();
}

std::shared_ptr<const Table> AbstractOperator::get_output() const {
  Assert(_output, "Operator not executed. Output is NULL.");
  return _output;
}

std::shared_ptr<const Table> AbstractOperator::_left_input_table() const {
  return _left_input->get_output();
}

std::shared_ptr<const Table> AbstractOperator::_right_input_table() const {
  return _right_input->get_output();
}

}  // namespace opossum
