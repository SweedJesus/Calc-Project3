#pragma once

#include <vector>
#include <stack>
#include <queue>
#include <functional>
#include <exception>

#include "Logger.h"
#include "BigInt.h"
#include "Command.h"

#include "util.h"

// -----------------------------------------------------------------------------
// Declarations
// -----------------------------------------------------------------------------

namespace mesa
{
  template<class T>
    class Calc
    {
      public:
        using Logger                  = mesa::Logger;
        using DataT                   = T;
        using Command                 = mesa::Command<DataT>;
        using Operands                = typename Command::Operands;
        using ArbitraryCommand        = mesa::ArbitraryCommand<DataT>;
        using ParseNumCommand         = mesa::ParseNumCommand<DataT>;
        using UnaryOpCommand          = mesa::UnaryOpCommand<DataT>;
        using BinaryOpCommand         = mesa::BinaryOpCommand<DataT>;
        using ConsumerBinaryOpCommand = mesa::ConsumerBinaryOpCommand<DataT>;
        using Commands                = std::vector<Command*>;

        // Default copy constructor
        Calc(const Calc&) = delete;

        // Default copy assignment
        void operator=(const Calc&) = delete;

        //! Return singleton instance
        static Calc* instance();

        //! Get stdout logger
        Logger* stdLogger()
        { return m_stdLogger; }

        //! Set stdout logger
        void stdLogger(Logger* logger);

        //! Get errout logger
        Logger* errLogger()
        { return m_errLogger; }

        //! Set errout logger
        void errLogger(Logger* logger);

        void printHelp(std::ostream& os)
        { os << s_HELP; }

        //! Evaluate string as expression
        // Evaluate a string a single prefix notation mathematical expression.
        // @throws runtime_error Token went unhandled, or operand stack has more
        // than one remaining.
        DataT evaluate(const std::string& line);

      private:
        Calc() { initialize(); }

        void initialize();

        static Calc *s_instance;
        static std::string s_HELP;

        Commands m_commands;
        Logger *m_stdLogger, *m_errLogger;
        Operands m_operands;
        DataT m_result;
    };
}

// -----------------------------------------------------------------------------
// Definitions
// -----------------------------------------------------------------------------

template<class T> mesa::Calc<T>* mesa::Calc<T>::s_instance = nullptr;

  template<class T>
void mesa::Calc<T>::initialize()
{
  auto add =
    [](DataT lhs, const DataT &rhs) { return lhs += rhs; };
  auto subtract =
    [](DataT lhs, const DataT &rhs) { return lhs -= rhs; };
  auto multiply =
    [](DataT lhs, const DataT &rhs) { return lhs *= rhs; };
  auto divide =
    [](DataT lhs, const DataT &rhs) { return lhs /= rhs; };
  auto modulus =
    [](DataT lhs, const DataT &rhs) { return lhs %= rhs; };
  auto exponentiate =
    [](DataT lhs, const DataT &rhs) { return lhs ^= rhs; };
  auto min =
    [](const DataT &lhs, const DataT &rhs)
    { return (lhs < rhs ? lhs : rhs); };
  auto max =
    [](const DataT &lhs, const DataT &rhs)
    { return (lhs > rhs ? lhs : rhs); };
  auto lcm =
    [](const DataT &lhs, const DataT &rhs)
    {
      DataT n = lhs;
      while ((n % rhs) != 0) {
        n += lhs;
      }
      return n;
    };
  auto gcf =
    [](const DataT &lhs, const DataT &rhs)
    {
      DataT n = (lhs > rhs ? lhs : rhs) / 2;
      while (!((lhs % n == 0) && (rhs % n == 0))) { --n; }
      return n;
    };

  // TODO: Update help
  m_commands = {
    new ParseNumCommand{},
    // Arbitrary commands
    new ArbitraryCommand{"ans", [this](const std::string&)
      {
        m_operands.push(m_result);
      }
    },
    // Binary operation commands
    new BinaryOpCommand{"+",   add},
    new BinaryOpCommand{"-",   subtract},
    new BinaryOpCommand{"*",   multiply},
    new BinaryOpCommand{"/",   divide},
    new BinaryOpCommand{"%",   modulus},
    new BinaryOpCommand{"^",   exponentiate},
    new BinaryOpCommand{"min", min},
    new BinaryOpCommand{"max", max},
    new BinaryOpCommand{"lcm", lcm},
    new BinaryOpCommand{"gcf", gcf},
    // Unary commands
    new UnaryOpCommand{"!", [](const DataT &lhs)
      {
        DataT result = lhs;
        for (auto i = (lhs-1); i > 1; --i)
          result *= i;
        return  result;
      }
    },
    // Consumer binary commands
    new ConsumerBinaryOpCommand{"+.",   add},
    new ConsumerBinaryOpCommand{"-.",   subtract},
    new ConsumerBinaryOpCommand{"*.",   multiply},
    new ConsumerBinaryOpCommand{"/.",   divide},
    new ConsumerBinaryOpCommand{"%.",   modulus},
    new ConsumerBinaryOpCommand{"^.",   exponentiate},
    new ConsumerBinaryOpCommand{"min.", min},
    new ConsumerBinaryOpCommand{"max.", min},
    new ConsumerBinaryOpCommand{"max.", max},
    new ConsumerBinaryOpCommand{"lcm.", lcm},
    new ConsumerBinaryOpCommand{"gcf.", gcf},
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
  m_stdLogger->log(LogLevel::Debug, "[Calc::evaluate] '" + line + "'\n");

  while (!m_operands.empty())
    m_operands.pop();

  // Convert space-delimited raw input line into token queue
  std::queue<std::string> tokens = queuify(line);
  std::string token;

  bool unhandled;
  try {
    while (!tokens.empty()) {
      unhandled = true;
      token = tokens.front();
      for (auto command: m_commands) {
        if (command->execute(m_operands, token)) {
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
    if (m_operands.size() == 0) {
      throw std::runtime_error(
          "No operands remaining on stack after evaluation, expected one");
    }
    else if (m_operands.size() > 1) {
      throw std::runtime_error(
          "More than one operand remaining on stack, expected one");
    }
  } catch (std::exception& e) {
    // Rethrow exception with stack-dump
    throw std::runtime_error(std::string{e.what()} +
        "\nStack dump: { " + stack_to_string(m_operands) + " }");
  }
  m_result = m_operands.top();
  return m_result;
}

