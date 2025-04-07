#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ConditionChecker.hpp>
#include <RandGen.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

class ParserJSON {
public:
  ParserJSON(const std::string& path);
  
  const std::vector<DataObject>& get_data() const;

  std::vector<DataObject> get_pattern() const;
  std::vector<DataObject> get_generated() const;

  std::string to_string() const;
  void generate_values(bool check_for_conditions = true);

private:
  std::shared_ptr<ConditionChecker> m_checker;
  bool has_solutions;
  RegexStringGenerator m_generator;


  std::vector<DataObject> m_data;
  std::vector<DataObject> m_generated;
  std::vector<std::shared_ptr<ConditionObject>> m_conditions;

  void generate_with_condition();
};