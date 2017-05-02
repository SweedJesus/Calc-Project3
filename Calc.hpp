#pragma once

#include <array>
#include <vector>
#include <functional>

#include "Logger.hpp"
#include "BigInt.hpp"
#include "Command.hpp"

namespace mesa
{
  // ---------------------------------------------------------------------------
  class Calc
  {
    public:
      using Logger = mesa::Logger;
      using DataT = mesa::BigInt;
      using Command = mesa::Command<DataT>;
      using OperandsT = Command::OperandsT;
      using BinaryOpCommand = mesa::BinaryOpCommand<DataT>;
      using ParseNumCommand = mesa::ParseNumCommand<DataT>;

      /* Commands needed (number of which as array size):
       *  - add
       * (- subtract)
       *  - multiply
       * (- divide)
       *  - exponentiate
       *  - parse number
       */
      using CommandsT = std::array<Command*, 6>;

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
      void stdLogger(Logger* stdLogger)
      { m_stdLogger = stdLogger; }

      /** Get errout logger
       */
      Logger* errLogger()
      { return m_errLogger; }

      /** Set errout logger
       */
      void errLogger(Logger* errLogger)
      { m_errLogger = errLogger; }

      /** Evaluate string as expression
       * Evaluate a string a single prefix notation mathematical expression.
       * @throws runtime_error Invalid operator
       * @throws invalid_argument Failed to convert an argument
       * @throws range_error Negative result
       */
      BigInt evaluate(const std::string& line);

    private:
      Calc();
      void initialize();

      static Calc *s_instance;

      CommandsT m_commands;

      Logger *m_stdLogger, *m_errLogger;
  };
  // ---------------------------------------------------------------------------
}
