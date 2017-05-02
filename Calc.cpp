#include <sstream>
#include <functional>
#include <stdexcept>

#include "Calc.hpp"
//#include "Logger.hpp"
//#include "BigInt.hpp"
//#include "Command.hpp"

using mesa::Logger;
using mesa::BigInt;
using mesa::Calc;
//using DataT = mesa::BigInt;
//using Command = mesa::Command<DataT>;
//using OperandsT = Command::OperandsT;
//using BinaryOpCommand = mesa::BinaryOpCommand<DataT>;
//using ParseNumCommand = mesa::ParseNumCommand<DataT>;

/*
 *inline std::string stack_to_string(OperandsT stack)
 *{
 *  std::ostringstream oss;
 *  while (!stack.empty()) {
 *    oss << stack.top();
 *    if (stack.size() > 1); oss << ' ';
 *    stack.pop();
 *  }
 *  return oss.str();
 *}
 */

Calc* Calc::s_instance = nullptr;

Calc::Calc()
{
  //initialize();
}

void Calc::initialize()
{
  // Populate command vector
  m_commands = {
    new BinaryOpCommand{'+', [](DataT lhs, const DataT& rhs) { return lhs += rhs; }},
    new BinaryOpCommand{'-', [](DataT lhs, const DataT& rhs) { return lhs -= rhs; }},
    new BinaryOpCommand{'*', [](DataT lhs, const DataT& rhs) { return lhs *= rhs; }},
    new BinaryOpCommand{'/', [](DataT lhs, const DataT& rhs) { return lhs /= rhs; }},
    new BinaryOpCommand{'^', [](DataT lhs, const DataT& rhs) { return lhs ^= rhs; }},
    new ParseNumCommand{}
  };
}

//Calc& Calc::instance()
//{
  //static Calc instance;
  //return instance;
//}

Calc* Calc::instance()
{
  if (!s_instance)
    s_instance = new Calc();;
  return s_instance;
}

BigInt Calc::evaluate(const std::string& line)
{
  // TODO: NEW -----------------------------------------------------------------
  /*
   *  // Operand stack
   *  std::stack<DataT> operands;
   *
   *  // Command objects
   *  BinaryOpCommand add{
   *    '+', [](DataT lhs, const DataT& rhs) { return lhs += rhs; }};
   *  BinaryOpCommand subtract{
   *    '-', [](DataT lhs, const DataT& rhs) { return lhs -= rhs; }};
   *  BinaryOpCommand multiply{
   *    '*', [](DataT lhs, const DataT& rhs) { return lhs *= rhs; }};
   *  BinaryOpCommand divide{
   *    '/', [](DataT lhs, const DataT& rhs) { return lhs /= rhs; }};
   *  BinaryOpCommand exponentiate{
   *    '^', [](DataT lhs, const DataT& rhs) { return lhs ^= rhs; }};
   *  ParseNumCommand parse;
   *
   *  // Commands (chain)
   *  std::vector<Command*> commands = {
   *    &add,
   *    &multiply,
   *    &exponentiate,
   *    &parse,
   *  };
   *
   *  // Create logger
   *  ConsoleLogger logger(&std::cout,
   *      LogLevel::Info | LogLevel::Error | LogLevel::Debug);
   *
   *  // Assign logger
   *  for (auto command: commands)
   *    command->stdoutLogger(&logger);
   *
   *  // Evaluate
   *  bool unhandled;
   *  for (auto token: input) {
   *    try {
   *      unhandled = true;
   *      for (auto command: commands) {
   *        if (command->execute(operands, token)) {
   *          unhandled = false;
   *          break;
   *        }
   *      }
   *      if (unhandled) {
   *        break;
   *      }
   *      // Could not be handled
   *    } catch (std::exception& e) {
   *      // TODO:
   *      std::cout << "Exception!\n  what():  " <<  e.what() << "\n";
   *    }
   *  }
   *
   *  if (!operands.empty()) {
   *    std::cout << "Remaining stack [ " << stack_to_string(operands) << "]\n";
   *  }
   */

  // TODO: OLD -----------------------------------------------------------------
  /*  std::istringstream iss{line};
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
  return BigInt{1};
}
