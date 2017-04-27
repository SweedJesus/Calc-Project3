#pragma once

#include <array>
#include <vector>
#include <functional>

#include "BigInt.hpp"
#include "Command.hpp"

namespace mesa
{
  // ---------------------------------------------------------------------------
  class Calculator
  {
    public:
      using DataT = mesa::BigInt;
      using OperandsT = Command<DataT>::OperandsT;
      using Command = mesa::Command<DataT>;
      using BinaryOpCommand = mesa::BinaryOpCommand<DataT>;
      using ParseNumCommand = mesa::ParseNumCommand<DataT>;

      /* Commands needed (number of which as array size):
       *  - Add
       * (- subtract)
       *  - Multiply
       * (- divide)
       *  - Exponentiate
       *  - Parse number
       */
      using CommandsT = std::array<Command*, 6>;

      Calculator* get_instance()
      { return &s_calc; }

      /** Evaluate string as expression
       * Evaluate a string a single prefix notation mathematical expression.
       * @throws runtime_error Invalid operator
       * @throws invalid_argument Failed to convert an argument
       * @throws range_error Negative result
       */
      BigInt evaluate(const std::string& line);

    private:
      Calculator()
      { initialize(); }

      void initialize();

      CommandsT m_commands;

      static Calculator s_calc;
  };
  // ---------------------------------------------------------------------------
}
