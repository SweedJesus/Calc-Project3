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

#include "Logger.hpp"
#include "BigInt.hpp"
#include "Command.hpp"
#include "Calc.hpp"

// Logger aliases
using LogLevel = mesa::LogLevel;
using StreamLogger = mesa::StreamLogger;
//using FileLogger = mesa::FileLogger;

// Data type alias
//using DataT = unsigned int;
//using DataT = mesa::BigInt;

// Command aliases
//using Command = mesa::Command<DataT>;
//using OperandsT = Command::OperandsT;

// Arithmetic command aliases
//using BinaryOpCommand = mesa::BinaryOpCommand<DataT>;
//using ParseNumCommand = mesa::ParseNumCommand<DataT>;

// Calculator aliases
using mesa::Calc;

// @return True if string contains only "valid" characters
/*
 *inline bool is_valid_string(const std::string& s)
 *{
 *  for (auto c: s) {
 *    switch (c) {
 *      case '0': case '1': case '2': case '3': case '4':
 *      case '5': case '6': case '7': case '8': case '9':
 *      case '+': case '*': case '^':
 *        break;
 *      default:
 *        return false;
 *        break;
 *    }
 *  }
 *  return true;
 *}
 */

// @return Vector of space delimited token strings from an input string
/*
 *inline std::vector<std::string> vectorize(const std::string& s)
 *{
 *  std::vector<std::string> tokens;
 *  std::istringstream iss{s};
 *  std::string temp;
 *  while (iss >> temp >> std::ws)
 *    //if (is_valid_string(temp))
 *    tokens.push_back(temp);
 *  //else
 *  //throw std::invalid_argument{temp};
 *  return tokens;
 *}
 */

// @return Queue of space delimited token strings from an input string
/*
 *inline std::queue<std::string> queuify(const std::string& s)
 *{
 *  std::queue<std::string> tokens;
 *  std::istringstream iss{s};
 *  std::string temp;
 *  while (iss >> temp >> std::ws)
 *    if (is_valid_string(temp))
 *      tokens.push(temp);
 *    else
 *      throw std::invalid_argument{temp};
 *  return tokens;
 *}
 */

int main(int argc, char* argv[])
{
  bool is_interactive = (isatty(0) && isatty(1));
  bool is_verbose = false;

  // Process program options
  for (char c; (c = getopt(argc, argv, "hv")) != -1;) {
    switch (c) {
      case 'h':
        std::cout <<
          "Project 3: PostFixCalculator\n"
          "Program options:\n"
          "  -h [ --help ]     Show this message\n"
          "  -v [ --verbose ]  Be verbose\n";
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

  // Logger
  StreamLogger logger{&std::cout, LogLevel::Info};

  // Calculator
  Calc* calc = Calc::instance();
  calc->stdLogger(&logger);
  calc->errLogger(&logger);

  // Input
  std::string line, token;

  // Print interactive intro message
  if (is_interactive)
    std::cout <<
      "Project 3: OO-Calc\n"
      "  Enter 'help' to display interactive usage information\n"
      "  Enter 'quit' to quit\n";

  // Main loop
  while (!std::cin.eof()) {
    if (is_interactive) std::cout << "> ";

    // Input
    line.clear();
    std::getline(std::cin, line);
    if (line.empty()) continue;

    // Parse optional command/comment
    std::istringstream{line} >> token;
    if (token[0] == '#') {
      continue;
    } else if (token == "q" || token == "quit") {
      std::cout << "Quitting...\n";
      break;
    } else if (token == "h" || token == "help" || token == "?") {
      std::cout <<
        "Help\n"
        "  [command] [operation <args...>]\n\n"
        "Commands:\n"
        "  q [ quit ]     Quit the program\n"
        "  h [ help, ? ]  Print this message\n\n"
        "Instructions:\n"
        "  Calculates the result of a single-line compound infix mathematical "
        "expressions. Operations are binary (expect to prior numerical "
        "arguments) and all are separated by whitespace.\n\n"
        "Supported operations:\n"
        "  +  Addition\n"
        "  -  Subraction\n"
        "  *  Multiplication\n"
        "  /  Division\n"
        "  ^  Exponentiation\n";
      continue;
    }

    // Execute and output
    if (is_verbose)
      std::cout << '"' << line << "\" = ";
    std::cout << calc->evaluate(line) << "\n";
  }

  return 0;
}
