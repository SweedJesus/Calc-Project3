// @author Nils Olsson

#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <queue>
#include <stack>
#include <string>

#include "BigInt.hpp"
#include "Command.hpp"
#include "Logger.hpp"

// Data type alias
//using DataT = unsigned int;
using DataT = mesa::BigInt;

// Command aliases
using Command = mesa::Command<DataT>;
using OperandsT = Command::OperandsT;

// Arithmetic command aliases
using BinaryOpCommand = mesa::BinaryOpCommand<DataT>;
using ParseNumCommand = mesa::ParseNumCommand<DataT>;

// Logger aliases
using LogLevel = mesa::LogLevel;
using ConsoleLogger = mesa::ConsoleLogger;
using FileLogger = mesa::FileLogger;

// @return True if string contains only "valid" characters
inline bool is_valid_string(const std::string& s)
{
  for (auto c: s) {
    switch (c) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case '+': case '*': case '^':
        break;
      default:
        return false;
        break;
    }
  }
  return true;
}

// @return Vector of space delimited token strings from an input string
inline std::vector<std::string> vectorize(const std::string& s)
{
  std::vector<std::string> tokens;
  std::istringstream iss{s};
  std::string temp;
  while (iss >> temp >> std::ws)
    //if (is_valid_string(temp))
    tokens.push_back(temp);
  //else
  //throw std::invalid_argument{temp};
  return tokens;
}

// @return Queue of space delimited token strings from an input string
inline std::queue<std::string> queuify(const std::string& s)
{
  std::queue<std::string> tokens;
  std::istringstream iss{s};
  std::string temp;
  while (iss >> temp >> std::ws)
    if (is_valid_string(temp))
      tokens.push(temp);
    else
      throw std::invalid_argument{temp};
  return tokens;
}

inline std::string stack_to_string(OperandsT stack)
{
  std::ostringstream oss;
  while (!stack.empty()) {
    oss << stack.top();
    if (stack.size() > 1); oss << ' ';
    stack.pop();
  }
  return oss.str();
}

int main(int argc, char* argv[])
{
  bool is_verbose = false;

  // Process program options
  for (char c; (c = getopt(argc, argv, "hv")) != -1;) {
    switch (c) {
      case 'h':
        std::cout <<
          "Project 3: PostFixCalculator\n"
          "Program options:\n"
          "  -h [ --help ]     Show this message\n"
          "  -v [ --verbose ]  Be verbose\n"
          "Example usage:\n"
          "  './Calc'                   "
          "Run in interactive mode\n"
          "  'echo \"^ 2 99\" | ./calc'   "
          "Echo expressions to be evaluated, as standard input\n"
          "  './calc < input.txt'       "
          "Redirect file containing expressions to be evaluated, as standard input\n";
        return 0;
        break;
      case 'v':
        is_verbose = true;
        break;
      default:
        std::cout
          << "Error: Invalid program option '" << c << "'\n";
        break;
    }
  }

  // Input
  std::string raw_input{"3 2 * 4 ^"};
  std::cout << "Raw input: \"" << raw_input << "\"\n";
  auto input = vectorize(raw_input);

  // Operand stack
  std::stack<DataT> operands;

  // Command objects
  BinaryOpCommand add{
    '+', [](DataT lhs, const DataT& rhs) { return lhs += rhs; }};
  BinaryOpCommand subtract{
    '-', [](DataT lhs, const DataT& rhs) { return lhs -= rhs; }};
  BinaryOpCommand multiply{
    '*', [](DataT lhs, const DataT& rhs) { return lhs *= rhs; }};
  BinaryOpCommand divide{
    '/', [](DataT lhs, const DataT& rhs) { return lhs /= rhs; }};
  BinaryOpCommand exponentiate{
    '^', [](DataT lhs, const DataT& rhs) { return lhs ^= rhs; }};
  ParseNumCommand parse;

  // Commands (chain)
  std::vector<Command*> commands = {
    &add,
    &multiply,
    &exponentiate,
    &parse,
  };

  // Create logger
  ConsoleLogger logger(&std::cout,
      LogLevel::Info | LogLevel::Error | LogLevel::Debug);

  // Assign logger
  for (auto command: commands)
    command->stdoutLogger(&logger);

  // Evaluate
  bool unhandled;
  for (auto token: input) {
    try {
      unhandled = true;
      for (auto command: commands) {
        if (command->execute(operands, token)) {
          unhandled = false;
          break;
        }
      }
      if (unhandled) {
        break;
      }
      // Could not be handled
    } catch (std::exception& e) {
      // TODO:
      std::cout << "Exception!\n  what():  " <<  e.what() << "\n";
    }
  }

  if (!operands.empty()) {
    std::cout << "Remaining stack [ " << stack_to_string(operands) << "]\n";
  }

  return 0;
}
