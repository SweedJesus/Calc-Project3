#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <queue>

namespace mesa
{
  inline bool is_numeric(const std::string& s)
  {
    for (auto c: s)
      if (c < '0' || c > '9')
        return false;
    return true;
  }

  // @return Vector of space delimited token strings from an input string
  inline std::vector<std::string> vectorify(const std::string& s)
  {
    std::vector<std::string> tokens;
    std::istringstream iss{s};
    std::string temp;
    while (iss >> temp >> std::ws)
      tokens.push_back(temp);
    //throw std::invalid_argument{temp};
    return tokens;
  }

  // @return Stackify of space delimited token strings from an input string
  inline std::stack<std::string> stackify(const std::string& s)
  {
    std::stack<std::string> tokens;
    std::istringstream iss{s};
    std::string temp;
    while (iss >> temp >> std::ws)
      tokens.push(temp);
    return tokens;
  }

  // @return Queue of space delimited token strings from an input string
  inline std::queue<std::string> queuify(const std::string& s)
  {
    std::queue<std::string> tokens;
    std::istringstream iss{s};
    std::string temp;
    while (iss >> temp >> std::ws)
      tokens.push(temp);
    return tokens;
  }

  // @return Contents of stack as a string
  template<class T>
  inline std::string stack_to_string(std::stack<T> stack)
  {
    std::ostringstream oss;
    oss << "(T..B) ";
    while (!stack.empty()) {
      oss << stack.top();
      if (stack.size() > 1) oss << ", ";
      stack.pop();
    }
    return oss.str();
  }
}
