#include <ParserJSON.hpp>

//TODO: remove
#include <iostream>

using json = nlohmann::json;

std::vector<DataObject> ParserJSON::get_pattern() const { return m_data; }

std::vector<DataObject> ParserJSON::get_generated() const { return m_generated; }

ParserJSON::ParserJSON(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  json json_data;
  file >> json_data;

  if (!json_data.contains("data") || !json_data["data"].is_array()) {
    throw std::runtime_error("Invalid JSON format: missing 'data' array");
  }

  for (const auto& item : json_data["data"]) {
    if (!item.is_object() || !item.contains("type") || !item.contains("value")) {
      continue;
    }

    std::string type_str = item["type"];
    std::string value = item["value"];
    std::string name = item.value("name", "");

    DataType type;
    if (type_str == "text") {
      type = DataType::TEXT;
    }
    else if (type_str == "regex") {
      type = DataType::REGEX;
    }
    else {
      continue;
    }

    m_data.emplace_back(type, value, name);
  }
  if (json_data.contains("conditions")) {
    if (!json_data["conditions"].is_array()) {
      throw std::runtime_error("\"conditions\" should be an array");
    }

    m_checker = std::make_shared<ConditionChecker>(ConditionChecker());

    for (const auto& condition : json_data["conditions"]) {
      if (!condition.is_object() || !condition.contains("type") || !condition.contains("names")) {
        continue;
      }

      ConditionType type = get_condition_type(condition["type"]);
      std::vector<std::string> names = {};

      if (!condition["names"].is_array()) {
        throw std::runtime_error("\"names\" should be an array");
      }

      for (const auto& name : condition["names"]) {
        names.push_back(name);
      }

      m_conditions.push_back(std::make_shared<ConditionObject>(ConditionObject(type, names)));
    }
  }
}

const std::vector<DataObject>& ParserJSON::get_data() const {
  return m_data;
}

std::string ParserJSON::to_string() const {
  std::string res = {};
  for (const auto& i : m_generated) {
    res += i.get_value();
  }
  return res;
}

void ParserJSON::generate_with_condition() {
  bool satisfied = false;

  while (!satisfied) {
    generate_values(false);
    bool total_results = true;
    for (const auto& condition : m_conditions) {

      auto type = condition->get_type();
      auto names = condition->get_names();

      std::vector<std::string> names_vec;
      for (const auto& name : names) {
        names_vec.push_back(name);
      }
      total_results &= (*m_checker)[type](m_generated, names_vec);
    }
    satisfied = total_results;

    // TODO: remove
    if (!satisfied) {
      std::cout << "Condition were not satisfied" << std::endl;
    }
  }
}

void ParserJSON::generate_values(bool check) {

  if (check && m_checker) {
    generate_with_condition();
    return;
  }

  m_generated = {};

  for (const auto& obj : m_data) {
    if (obj.get_type() == DataType::TEXT) {
      m_generated.push_back(obj);
    }
    else {
      m_generated.emplace_back(DataType::TEXT, m_generator.generate(obj.get_value()), obj.get_name());
    }
  }
}