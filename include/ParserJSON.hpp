#pragma once

#include <string>
#include <vector>

class ParserJSON {
public:
  enum class DataType {
    TEXT,
    REGEX
  };
  
  struct DataObject {
    DataObject(DataType type, const std::string& value, const std::string& name = "");
    
    DataType get_type() const;
    std::string get_value() const;
    std::string get_name() const;
  private:
    DataType type;
    std::string value;
    std::string name;
  };
  
  ParserJSON(const std::string& path);
  
  const std::vector<DataObject>& get_data() const;
  
private:
  std::vector<DataObject> m_data;  // Вектор с данными
};