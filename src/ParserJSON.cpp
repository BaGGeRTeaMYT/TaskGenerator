#include <ParserJSON.hpp>

ParserJSON::ParserJSON(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл: " + path);
  }

  json json_data;
  file >> json_data;

  if (json_data.contains("tasks")) {
    if (!json_data["tasks"].is_array()) {
      incorrect_format_error("объект \"tasks\" должен быть массивом");
    }
    for (const auto& task : json_data["tasks"]) {
      parse_data(task);
    }
  }
  else {
    parse_data(json_data);
  }

  double total_prob = 1;
  int probs_to_default = 0;
  for (auto prob : m_probabilities) {
    if (prob >= 0) {
      total_prob -= prob;
    }
    else {
      probs_to_default++;
    }
  }
  if (total_prob < 0) {
    incorrect_format_error("сумма \"probablity\" не должна превосходить 1");
  }
  for (auto& prob : m_probabilities) {
    if (prob < 0) {
      prob = total_prob / probs_to_default;
    }
  }
  total_prob = 0;
  m_cdf = {};
  for (auto prob : m_probabilities) {
    total_prob += prob;
    if (total_prob > 1 - std::numeric_limits<double>::epsilon()) {
      total_prob = 1;
    }
    m_cdf.insert(total_prob);
  }
}

std::vector<std::vector<DataObject>> ParserJSON::get_data() const { return m_data; }

std::vector<std::vector<DataObject>> ParserJSON::get_pattern() const { return m_data; }

std::vector<DataObject> ParserJSON::get_generated() const { return m_generated; }

std::string ParserJSON::to_string() const {
  std::string res = {};
  for (const auto& i : m_generated) {
    res += i.get_value();
  }
  return res;
}

void ParserJSON::generate_to_file(int amount, const std::string& path) {
  std::ofstream output(path);
  output.clear();
  output <<
    R"(\documentclass[11pt]{article}
\usepackage{amsmath, amsfonts, amsthm}
\usepackage[utf8]{inputenc}
\usepackage[T2A]{fontenc}
\usepackage[russian]{babel}
\usepackage{fontspec}
\setmainfont{Times New Roman}

\begin{document}
)";

  for (int i = 1; i <= amount; i++) {
    generate_values();
    output << "\n\\section*{Вариант "
      << i
      << "}\n";
    output << to_string();
  }

  output <<
    R"(
\end{document}
% generated with task generator)";
}

void ParserJSON::parse_data(const json& json_data) {

  m_data.push_back({});
  m_solutions.push_back({});
  m_conditions.push_back({});

  if (!json_data.contains("data")) {
    incorrect_format_error("объект \"data\" обязателен");
  }

  if (!json_data["data"].is_array()) {
    incorrect_format_error("объект \"data\" должен быть массивом");
  }

  for (const auto& item : json_data["data"]) {
    if (!item.is_object() || !item.contains("type") || !item.contains("value")) {
      continue;
    }

    std::string type_str = item["type"];
    std::string value = item["value"];
    std::string name = item.value("name", "");

    DataType type = get_data_type(type_str);

    m_data.back().emplace_back(type, value, name);
  }
  if (json_data.contains("conditions")) {
    if (!json_data["conditions"].is_array()) {
      incorrect_format_error("объект \"conditions\" должен быть массивом");
    }

    m_checker = std::make_shared<ConditionChecker>(ConditionChecker());

    for (const auto& condition : json_data["conditions"]) {
      if (!condition.is_object() || !condition.contains("type") || !condition.contains("names")) {
        continue;
      }

      ConditionType type = get_condition_type(condition["type"]);
      std::vector<std::string> names = {};

      if (!condition["names"].is_array()) {
        incorrect_format_error("объект \"names\" должен быть массивом");
      }

      for (const auto& name : condition["names"]) {
        names.push_back(name);
      }

      m_conditions.back().push_back(std::make_shared<ConditionObject>(ConditionObject(type, names)));
    }

  }

  if (json_data.contains("solution")) {
    if (!json_data["solution"].is_array()) {
      incorrect_format_error("объект \"solution\" должен быть массивом");
    }
    for (const auto& sol : json_data["solution"]) {

    }
  }

  if (json_data.contains("probability")) {
    if (!json_data["probability"].is_number_float()) {
      incorrect_format_error("объект \"probability\" должен быть вещественным числом.");
    }
    double prob = json_data["probability"];
    if (prob < 0 || prob > 1) {
      incorrect_format_error("значение объекта \"probability\" должно быть от 0 до 1");
    }
    m_probabilities.push_back(prob);
  }
  else {
    m_probabilities.push_back(-1);
  }
}

void ParserJSON::generate_values(bool check) {
  current_variant = get_variant();
  if (check && m_checker) {
    generate_with_condition();
    return;
  }

  m_generated = {};

  for (const auto& obj : m_data[current_variant]) {
    if (obj.get_type() == DataType::TEXT) {
      m_generated.push_back(obj);
    }
    else {
      m_generated.emplace_back(DataType::TEXT, m_generator.generate(obj.get_value()), obj.get_name());
    }
  }
}

void ParserJSON::generate_with_condition() {
  bool satisfied = false;

  while (!satisfied) {
    generate_values(false);
    bool total_results = true;
    for (const auto& condition : m_conditions) {

      auto type = condition[current_variant]->get_type();
      auto names = condition[current_variant]->get_names();

      std::vector<std::string> names_vec;
      for (const auto& name : names) {
        names_vec.push_back(name);
      }
      total_results &= (*m_checker)[type](m_generated, names_vec);
    }
    satisfied = total_results;
  }
}

void ParserJSON::incorrect_format_error(const std::string& what) const {
  throw std::runtime_error("Некорректный формат файла конфигурации: " + what);
}

int ParserJSON::get_variant() {
  int num = 0;
  double prob = m_num_gen.get_num();
  for (const auto cdf_val : m_cdf) {
    if (prob > cdf_val) {
      num++;
    }
  }
  return num;
}