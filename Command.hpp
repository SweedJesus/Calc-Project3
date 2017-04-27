#pragma once

/*
 * 1. Post-fix notation ("a b +" == "a + b")
 *   - no operator precedence
 *     - "((a + b) * c)" == "a b + c *"
 * 2. Chain of command pattern
 *   - no longer purely functional
 *   - abstract away actual operations
 *   - http://gameprogrammingpatterns.com/command.html
 *   - https://en.wikipedia.org/wiki/Chain-of-responsibility_pattern
 * 3. Encapsulate functionality as classes derived from Command
 *   - including operators, utility functions (string to number)
 *   - arithmetic logic confined to Command derived classes
 */

#include <string>
#include <stack>
#include <algorithm>
#include <cmath>

#include "Logger.hpp"

using LogLevel = mesa::LogLevel;
using Logger = mesa::Logger;

inline bool is_numeric(const std::string& s)
{
  return !(s.empty() || std::find_if(s.begin(), s.end(),
      [](char c) { return !std::isdigit(c); }) != s.end());
}

namespace mesa
{
  // ---------------------------------------------------------------------------

  /** Command interface
  */
  template<class T> class Command
  {
    public:
      // Type aliases
      using DataT     = T;
      using OperandsT = std::stack<DataT>;

      /** Destructor
      */
      virtual ~Command()
      {}

      void stdoutLogger(Logger* logger)
      { m_stdoutLogger = logger; }

      void stderrLogger(Logger* logger)
      { m_stderrLogger = logger; }

      /** Execute command
       * @param operands Stack of unsigned integer numbers.
       * @param token Token to be evaluated and used during execution. If the
       * token is not "appropriate" for this object then do nothing.
       * @return true if this command was executed.
       */
      virtual bool execute(
          OperandsT& operands,
          const std::string& token) const = 0;

    protected:
      void log(const LogLevel& logLevel, const std::string& line) const
      {
        if (m_stdoutLogger != nullptr)
          m_stdoutLogger->log(logLevel, line);
      }

      void elog(const LogLevel& logLevel, const std::string& line) const
      {
        if (m_stderrLogger != nullptr)
          m_stderrLogger->log(logLevel, line);
      }

      Logger* m_stdoutLogger = nullptr;
      Logger* m_stderrLogger = nullptr;
  };

  // ---------------------------------------------------------------------------

  template<class T> class BinaryOpCommand : public Command<T>
  {
    public:
      using DataT      = typename Command<T>::DataT;
      using OperandsT  = typename Command<T>::OperandsT;
      using OperationT = std::function<T(T, const T&)>;

      BinaryOpCommand(char opChar, OperationT op):
        m_OP_CHAR{opChar},
        m_op{op}
      {}

      bool execute(
          typename Command<T>::OperandsT& operands,
          const std::string& token) const override
      {
        if (token.size() == 1 && token[0] == this->m_OP_CHAR) {
          std::cout << "[BinaryOpCommand: '" << m_OP_CHAR << "']\n";
          if (operands.size() < 2) {
            throw std::runtime_error(
                "Binary operations require two operands");
          }
          auto lhs = operands.top(); operands.pop();
          auto rhs = operands.top(); operands.pop();
          operands.push(m_op(lhs, rhs));
          return true;
        }
        return false;
      }

    protected:
      const char m_OP_CHAR;
      OperationT m_op;
  };

  // ---------------------------------------------------------------------------

  /** Store as integer command
  */
  template<class T> class ParseNumCommand : public Command<T>
  {
    public:
      using DataT     = typename Command<T>::DataT;
      using OperandsT = typename Command<T>::OperandsT;

      bool execute(
          typename Command<T>::OperandsT& operands,
          const std::string& token) const override
      {
        // Return if empty or invalid
        if (token.empty() || std::find_if(token.begin(), token.end(),
              [](char c) { return !std::isdigit(c); }) != token.end())
          return false;
        Command<T>::log(LogLevel::Debug, "[ParseNumCommand]\n");
        //operands.push(std::stoi(token));
        operands.emplace(token);
        return true;
      }
  };
}
