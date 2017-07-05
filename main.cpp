// @author Nils Olsson
//
// Now using the GNU readline and history libraries for improved interface
// https://cnswww.cns.cwru.edu/php/chet/readline/readline.html
// https://cnswww.cns.cwru.edu/php/chet/readline/history.html

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include "cpp-readline/src/Console.hpp"
#include <iostream>
#include <string>

#include "Logger.h"
#include "Command.h"
#include "Calc.h"
#include "BigInt.h"

// Logger aliases
using LogLevel     = mesa::LogLevel;
using StreamLogger = mesa::StreamLogger;
using FileLogger   = mesa::FileLogger;

// Data type alias
using Data = mesa::BigInt;

// Calculator aliases
using Calc = mesa::Calc<Data>;

const std::string HELP =
"Help\n"
"  [command] [operation <args...>]\n"
"\n"
"Commands:\n"
"  q [ quit ]     Quit the program\n"
"  h [ help, ? ]  Print this message\n"
"\n"
"Instructions:\n"
"  Calculates the result of a single-line compound post-fix mathematical "
"expressions."
"Binary operations and commands consume and expect two operands, while unary"
"operations consume only one one. Additionally, consumer commands will consume"
"all operands on the stack by applying the equivalent binary operation until"
"only a single result is left on the stack. And lastly, arbitrary commands"
"provide special functionality while requiring no operands.\n"
"\n"
"Binary operations:\n"
"  +    Addition\n"
"  -    Subraction\n"
"  *    Multiplication\n"
"  /    Division\n"
"  %    Modulus\n"
"  ^    Exponentiation\n"
"  max  Maximum of two values\n"
"  min  Minimum of two values\n"
"  lcm  Least common multiple\n"
"  gcf  Greatest common factor\n"
"\n"
"Unary operations:\n"
"  !    Factorial\n"
"\n"
"Consumer binary operations:\n"
"  ++   Addition\n"
"  --   Subtraction\n"
"  **   Multiplication\n"
"  //   Division\n"
"  %%   Modulus\n"
"  ^^   Exponentiation\n"
"\n"
"Arbitrary operations:\n"
"  ans  Answer of last expression\n";

// -----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  bool is_interactive = (isatty(0) && isatty(1));
  bool is_verbose = false;
  bool is_debug = false;
  bool is_running = true;

  // Process program options
  for (char c; (c = getopt(argc, argv, "hvd")) != -1;) {
    switch (c) {
      case 'h':
        std::cout <<
          "Project 3: PostFixCalculator\n"
          "Program options:\n"
          "  -h  Show this message\n"
          "  -v  Start in verbose mode\n"
          "  -d  Start in debug mode\n";
        return 0;
        break;
      case 'v':
        is_verbose = true;
        break;
      case 'd':
        is_debug = true;
        break;
      default:
        std::cout
          << "Error: Invalid program option '" << c << "'\n";
        return 1;
    }
  }

  // Logger
  size_t logLevel =
    (LogLevel::Info | is_debug * LogLevel::Debug);
  StreamLogger logger{&std::cout, logLevel};

  // Calculator
  Calc* calc = Calc::instance();
  calc->stdLogger(&logger);
  calc->errLogger(&logger);

  // Input containers
  std::string token, prompt;
  char* line;
  Data result;

  // Interactive mode message and line prompt
  if (is_interactive) {
    std::cout <<
      "Project 3: PostFixCalculator\n"
      "  Enter 'help' to display interactive usage information\n"
      "  Enter 'quit' to quit\n";
    prompt = "> ";
  }

  // Main loop
  while (is_running) {
    // GNU readline/history
    line = readline(prompt.c_str());
    if (line == NULL) {
      // Break at end of standard-input
      break;
    } else if (line[0] == '\0') {
      // Skip empty line
      continue;
    }
    add_history(line);

    // Parse optional command/comment
    std::istringstream{line} >> token;
    if (token[0] == '#') {
      continue;
    } else if (token == "q" || token == "quit") {
      std::cout << "Quitting...\n";
      is_running = false; // Not really doing anything, but w/e
      break;
    } else if (token == "v" || token == "verbose") {
      is_verbose = !is_verbose;
      std::cout << (is_verbose ?
          "(Verbosity enabled)\n" :
          "(Verbosity disabled)\n");
      continue;
    } else if (token == "d" || token == "debug") {
      is_debug = !is_debug;
      std::cout << (is_debug ?
          "(Debugging enabled)\n" :
          "(Debugging disabled)\n");
      logger.logLevel(logger.logLevel() ^ LogLevel::Debug);
      continue;
    } else if (token == "h" || token == "help" || token == "?") {
      std::cout << HELP;
      continue;
    }

    // Execute and output
    try {
      result = calc->evaluate(line);
      if (is_verbose)
        std::cout << '"' << line << "\" = ";
      std::cout << result << "\n";
    } catch (std::exception& e) {
      std::cout << "Exception!\n  what():  " << e.what() << "\n";
    }
  }

  delete line;

  return 0;
}
