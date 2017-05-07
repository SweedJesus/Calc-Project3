#pragma once

#include <vector>
#include <stack>
#include <queue>
#include <functional>
#include <exception>

#include "Logger.hpp"
#include "BigInt.hpp"
#include "Command.hpp"

#include "util.hpp"

// -----------------------------------------------------------------------------
// Declarations
// -----------------------------------------------------------------------------

namespace mesa
{
  template<class T>
  class Calc
  {
    public:
      using Logger           = mesa::Logger;
      using DataT            = T;
      using Command          = mesa::Command<DataT>;
      using OperandsT        = typename Command::OperandsT;
      using ParseNumCommand  = mesa::ParseNumCommand<DataT>;
      using UnaryOpCommand   = mesa::UnaryOpCommand<DataT>;
      using BinaryOpCommand  = mesa::BinaryOpCommand<DataT>;
      using ArbitraryCommand = mesa::ArbitraryCommand<DataT>;

      /* Commands needed (number of which as array size):
       *  - add
       * (- subtract)
       *  - multiply
       * (- divide)
       *  - exponentiate
       *  - parse number
       */
      using CommandsT = std::vector<Command*>;

      // Default copy constructor
      Calc(const Calc&) = delete;

      // Default copy assignment
      void operator=(const Calc&) = delete;

      /** Return singleton instance
      */
      static Calc* instance();

      /** Get stdout logger
      */
      Logger* stdLogger()
      { return m_stdLogger; }

      /** Set stdout logger
      */
      void stdLogger(Logger* logger);

      /** Get errout logger
      */
      Logger* errLogger()
      { return m_errLogger; }

      /** Set errout logger
      */
      void errLogger(Logger* logger);

      /** Evaluate string as expression
       * Evaluate a string a single prefix notation mathematical expression.
       * @throws runtime_error Token went unhandled, or operand stack has more
       * than one remaining.
       */
      DataT evaluate(const std::string& line);

    private:
      Calc() { initialize(); }

      void initialize();

      static Calc *s_instance;

      CommandsT m_commands;
      Logger *m_stdLogger, *m_errLogger;
  };
}

// -----------------------------------------------------------------------------
// Definitions
// -----------------------------------------------------------------------------

template<class T> mesa::Calc<T>* mesa::Calc<T>::s_instance = nullptr;

  template<class T>
void mesa::Calc<T>::initialize()
{
  // Populate command vector
  // TODO:
  // Factorial
  // Square root
  // 'x' root

  m_commands = {
    //new ArbitraryCommand{"hw", [](const std::string& lhs)
    //{ std::cout << "Hello world\n"; }},
    new ParseNumCommand{},
    new BinaryOpCommand{"+", [](DataT lhs, const DataT& rhs)
      { return lhs += rhs; }},
    new BinaryOpCommand{"-", [](DataT lhs, const DataT& rhs)
      { return lhs -= rhs; }},
    new BinaryOpCommand{"*", [](DataT lhs, const DataT& rhs)
      { return lhs *= rhs; }},
    new BinaryOpCommand{"/", [](DataT lhs, const DataT& rhs)
      { return lhs /= rhs; }},
    new BinaryOpCommand{"^", [](DataT lhs, const DataT& rhs)
      { return lhs ^= rhs; }},
    new UnaryOpCommand{"root", [](DataT lhs)
      { return ceil(1.0); }},
  };
}

  template<class T>
mesa::Calc<T>* mesa::Calc<T>::instance()
{
  if (!s_instance)
    s_instance = new Calc();
  return s_instance;
}

  template<class T>
void mesa::Calc<T>::stdLogger(Logger* logger)
{
  m_stdLogger = logger;
  for (auto command: m_commands)
    command->stdoutLogger(logger);
}

  template<class T>
void mesa::Calc<T>::errLogger(Logger* logger)
{
  m_errLogger = logger;
  for (auto command: m_commands)
    command->stderrLogger(logger);
}

  template<class T>
typename mesa::Calc<T>::DataT mesa::Calc<T>::evaluate(const std::string& line)
{
  OperandsT operands;

  // Convert space-delimited raw input line into token queue
  std::queue<std::string> tokens = queuify(line);
  std::string token;

  bool unhandled;
  try {
    while (!tokens.empty()) {
      unhandled = true;
      token = tokens.front();
      for (auto command: m_commands) {
        if (command->execute(operands, token)) {
          unhandled = false;
          break;
        }
      }
      if (unhandled) {
        throw std::runtime_error(
            "Token '" + token + "' went unhandled");
      }
      tokens.pop();
    }
    if (operands.size() == 0) {
      throw std::runtime_error(
          "No operands remaining on stack after evaluation, expected one");
    }
    else if (operands.size() > 1) {
      throw std::runtime_error(
          "More than one operand remaining on stack, expected one");
    }
  } catch (std::exception& e) {
    // Rethrow exception with stack-dump
    throw std::runtime_error(std::string{e.what()} +
        "\nStack dump: { " + stack_to_string(operands) + " }");
  }
  return operands.top();
}

