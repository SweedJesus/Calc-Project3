#pragma once

#include <vector>

#include "Command.hpp"
#include "Logger.hpp"

namespace mesa
{
  /** Black box
   */
  template<class T>
  class BlackBox
  {
    public:
      using CommandT = mesa::Command<T>;

      static BlackBox& instance()
      {
        return m_instance;
      }

      bool evaluate(const std::string& s)
      {
        return true;
      }

    private:
      BlackBox()
      {}

      static BlackBox m_instance;

      static std::vector<CommandT> m_commands;
  };
}
