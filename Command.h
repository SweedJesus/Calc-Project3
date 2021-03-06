// Nils Olsson

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

#include <stack>
#include <functional>
#include <algorithm>

#include "Logger.h"

#include "util.h"

namespace mesa
{
  // ---------------------------------------------------------------------------
  //! Command interface class
  template<class T> class Command
  {
    public:
      // Type aliases
      using Data     = T;
      using Operands = std::stack<Data>;

      virtual ~Command()                 = default;
      //Command(const Command&)            = default;
      //Command(Command&&)                 = default;
      //Command& operator=(const Command&) = default;
      //Command& operator=(Command&&)      = default;

      /** Get standard output logger
      */
      Logger* stdoutLogger()
      { return m_stdoutLogger; }

      /** Set standard output logger
      */
      void stdoutLogger(Logger* logger)
      { m_stdoutLogger = logger; }

      /** Get standard error logger
      */
      Logger* erroutLogger()
      { return m_stderrLogger; }

      /** Set standard error logger
      */
      void stderrLogger(Logger* logger)
      { m_stderrLogger = logger; }

      /** Execute command
       * @param operands Stack of unsigned integer numbers.
       * @param token Token to be evaluated and used during execution. If the
       * token is not "appropriate" for this object then do nothing.
       * @return true if this command was executed.
       */
      virtual bool execute(
          Operands& operands,
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
  //! Arbitrary command
  template<class T> class ArbitraryCommand : public Command<T>
  {
    public:
      using Data      = typename Command<T>::Data;
      using Operands  = typename Command<T>::Operands;
      using Operation = std::function<void(const std::string&)>;

      ArbitraryCommand(const std::string& token, Operation op):
        m_TOKEN{token},
        m_op{op}
      {}

      bool execute(
          Operands&,
          const std::string& token) const override
      {
        if (token != m_TOKEN)
          return false;
        Command<T>::log(LogLevel::Debug,
            "[ArbitraryCommand] token:'" + token + "'\n");
        m_op(token);
        return true;
      }

    protected:
      const std::string m_TOKEN;
      Operation m_op;
  };

  // ---------------------------------------------------------------------------
  //! "Parse operand as number" command
  template<class T> class ParseNumCommand : public Command<T>
  {
    public:
      using Data     = typename Command<T>::Data;
      using Operands = typename Command<T>::Operands;

      bool execute(
          Operands& operands,
          const std::string& token) const override
      {
        if (token.empty() || !mesa::is_numeric(token))
          return false;
        Command<T>::log(LogLevel::Debug,
            "[ParseNumCommand] token:'" + token +
            "' stack:{ " + stack_to_string(operands) + " }\n");
        operands.emplace(token);
        return true;
      }
  };

  // ---------------------------------------------------------------------------
  //! Unary operation command
  template<class T> class UnaryOpCommand : public Command<T>
  {
    public:
      using Data      = typename Command<T>::Data;
      using Operands  = typename Command<T>::Operands;
      using Operation = std::function<T(const T&)>;

      UnaryOpCommand(const std::string& token, Operation op):
        m_TOKEN{token},
        m_op{op}
      {}

      bool execute(
          Operands& operands,
          const std::string& token) const override
      {
        if (token == m_TOKEN) {
          Command<T>::log(LogLevel::Debug,
              "[UnaryOpCommand] token:'" + token +
              "' stack:{ " + stack_to_string(operands) + " }");
          if (operands.size() < 1) {
            Command<T>::log(LogLevel::Debug, "\n");
            throw std::runtime_error("Unary operation requires one operand");
          }
          auto lhs = operands.top(); operands.pop();
          auto result = m_op(lhs);
          Command<T>::log(LogLevel::Debug,
              " -> " + std::string{result} + "\n");
          operands.push(result);
          return true;
        }
        return false;
      }

    protected:
      const std::string m_TOKEN;
      Operation m_op;
  };

  // ---------------------------------------------------------------------------
  //! Binary operation command
  template<class T> class BinaryOpCommand : public Command<T>
  {
    public:
      using Data      = typename Command<T>::Data;
      using Operands  = typename Command<T>::Operands;
      using Operation = std::function<T(const T&, const T&)>;

      BinaryOpCommand(const std::string& token, Operation op):
        m_TOKEN{token},
        m_op{op}
      {}

      bool execute(
          Operands &operands,
          const std::string& token) const override
      {
        if (token != m_TOKEN)
          return false;
        Command<T>::log(LogLevel::Debug,
            "[BinaryOpCommand] token:'" + token +
            "' stack:{ " + stack_to_string(operands) + " }");
        if (operands.size() < 2) {
          Command<T>::log(LogLevel::Debug, "\n");
          throw std::runtime_error(
              "Binary operation require two operands");
        }
        auto rhs = operands.top(); operands.pop();
        auto lhs = operands.top(); operands.pop();
        auto result = m_op(lhs, rhs);
        Command<T>::log(LogLevel::Debug,
            " -> " + std::string{result} + "\n");
        operands.push(result);
        return true;
      }

    protected:
      const std::string m_TOKEN;
      Operation m_op;
  };

  // ---------------------------------------------------------------------------
  //! Consumer binary operation command
  template<class T> class ConsumerBinaryOpCommand : public Command<T>
  {
    public:
      using Data      = typename Command<T>::Data;
      using Operands  = typename Command<T>::Operands;
      using Operation = std::function<T(const T& lhs, const T& rhs)>;

      ConsumerBinaryOpCommand(const std::string &token, Operation op):
        m_TOKEN{token},
        m_op{op}
      {}

      bool execute(
          Operands &operands,
          const std::string& token) const override
      {
        if (token != m_TOKEN)
          return false;
        Command<T>::log(LogLevel::Debug,
            "[ConsumerBinaryOpCommand] token:'" + token +
            "' stack:{ " + stack_to_string(operands) + " }\n");
        if (operands.size() < 2) {
          Command<T>::log(LogLevel::Debug, "\n");
          throw std::runtime_error(
              "Consumer binary operation requires at least two operands");
        }
        Data result;
        while (operands.size() > 1) {
          result = operands.top(); operands.pop();
          result = m_op(result, operands.top()); operands.pop();
          operands.push(result);
        }
        return true;
      }

    protected:
      const std::string m_TOKEN;
      Operation m_op;
  };
}
