#pragma once

#include <string>
#include <vector>
#include <map>
#include <MyTypes.hpp>
#include <stdexcept>

class ConditionChecker {
public:
  using CheckerFunction = std::function<bool(const std::map<std::string, std::string>& var_values, const std::vector<std::string>& names)>;

  ConditionChecker();

  ConditionChecker::CheckerFunction operator[](ConditionType) const;

private:
  std::map<ConditionType, CheckerFunction> checkers;
};