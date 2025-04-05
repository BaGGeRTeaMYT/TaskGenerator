#include <ParserJSON.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ParserJSON::DataObject::DataObject(
  DataType type,
  const std::string& value,
  const std::string& name
): type(type), value(value), name(name) {}

ParserJSON::DataType ParserJSON::DataObject::get_type() const { return type; }

std::string ParserJSON::DataObject::get_value() const { return value; }

std::string ParserJSON::DataObject::get_name() const { return name; }

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
            continue; // Пропускаем невалидные элементы
        }

        std::string type_str = item["type"];
        std::string value = item["value"];
        std::string name = item.value("name", ""); // Если name нет, используем пустую строку

        DataType type;
        if (type_str == "text") {
            type = DataType::TEXT;
        } else if (type_str == "regex") {
            type = DataType::REGEX;
        } else {
            continue; // Пропускаем неизвестные типы
        }

        m_data.emplace_back(type, value, name);
    }
}

const std::vector<ParserJSON::DataObject>& ParserJSON::get_data() const {
    return m_data;
}

