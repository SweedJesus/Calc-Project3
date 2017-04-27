#include <sstream>
#include <functional>
#include <stdexcept>

#include "BigInt.hpp"
#include "Calc.hpp"

using mesa::Calculator;

using mesa::BigInt;

void Calculator::initialize()
{
  m_commands = {
    new BinaryOpCommand{ '+', [](DataT lhs, const DataT& rhs)
      { return lhs += rhs; }},
    new BinaryOpCommand{ '-', [](DataT lhs, const DataT& rhs)
      { return lhs -= rhs; }},
    new BinaryOpCommand{ '*', [](DataT lhs, const DataT& rhs)
      { return lhs *= rhs; }},
    new BinaryOpCommand{ '/', [](DataT lhs, const DataT& rhs)
      { return lhs /= rhs; }},
    new BinaryOpCommand{ '^', [](DataT lhs, const DataT& rhs)
      { return lhs ^= rhs; }},
    new ParseNumCommand{}
  };
}



mesa::BigInt mesa::Calculator::evaluate(const std::string& line)
{
  /*
   *  std::istringstream iss{line};
   *  operationT op;
   *  std::vector<BigInt> args;
   *  BigInt result;
   *  std::string token;
   *
   *  // Get operator
   *  iss >> token;
   *  if (token.size() == 1)
   *    op = get_operation(token[0]);
   *
   *  // Invalid operator
   *  if (!op)
   *    throw std::runtime_error("Invalid operator '" + token + "'");
   *
   *  // Parse arguments
   *  args.clear();
   *  while (!iss.eof() && (iss >> token >> std::ws)) {
   *    args.push_back(token);
   *  }
   *  if (iss.fail())
   *    throw std::invalid_argument("Invalid argument '" + token + "'");
   *  else if (args.empty()) {
   *    throw std::invalid_argument("No arguments specified");
   *  }
   *
   *  // Execute operation
   *  // Can throw range_error
   *  op(args, result);
   *
   *  return result;
   */
}
