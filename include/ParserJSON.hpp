#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ConditionChecker.hpp>
#include <RandGen.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <set>

class ParserJSON {
public:
  using json = nlohmann::json;

  ParserJSON(const std::string& path);

  std::vector<std::vector<DataObject>> get_data() const;

  std::vector<std::vector<DataObject>> get_pattern() const;
  std::vector<DataObject> get_generated() const;

  std::string to_string() const;
  void generate_to_file(int amount, const std::string& path);

private:
  int current_variant;
  std::shared_ptr<ConditionChecker> m_checker;
  RegexStringGenerator m_generator;
  UniformNumberGenerator m_num_gen;

  void parse_data(const json& json_data);
  void generate_values(bool check_for_conditions = true);

  std::vector<std::vector<DataObject>> m_data;
  std::vector<std::vector<DataObject>> m_solutions;
  std::vector<std::vector<std::shared_ptr<ConditionObject>>> m_conditions;
  std::vector<double> m_probabilities;
  std::set<double> m_cdf;

  std::vector<DataObject> m_generated;
  std::vector<DataObject> m_generated_solutions;

  int get_variant();

  void generate_with_condition();
  void incorrect_format_error(const std::string& what) const;
};