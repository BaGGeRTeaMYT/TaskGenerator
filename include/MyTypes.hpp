#include <string>
#include <array>
#include <cmath>
#include <limits>
#include <functional>
#include <stdexcept>
#include <vector>

enum class DataType {
  TEXT,
  REGEX,
  VAR
};

enum class ConditionType {
  FORM_PLANE
};

ConditionType get_condition_type(const std::string& str);
DataType get_data_type(const std::string& str);

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

struct ConditionObject {
  ConditionObject(const std::string& type, const std::vector<std::string>& names);
  ConditionObject(ConditionType type, const std::vector<std::string>& names);

  ConditionType get_type() const;
  std::vector<std::string> get_names() const;
private:
  ConditionType type;
  std::vector<std::string> names;
};

struct Point3D {
  double x, y, z;
  Point3D(const std::string& x_str, const std::string& y_str, const std::string& z_str);
  Point3D(const double x, const double y, const double z);
  Point3D();
};

struct Vector3D {
  double x, y, z;

  Vector3D(const Point3D& point);
  Vector3D(const Point3D& first, const Point3D& second);

  Vector3D cross_product(const Vector3D& other) const;

  Vector3D operator-(const Vector3D& other) const;

  Point3D as_point() const;
  double length() const;
};