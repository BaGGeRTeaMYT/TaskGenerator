#include <MyTypes.hpp>

void type_error(const std::string& str) {
  throw std::runtime_error("Type: " + str);
}

ConditionType get_condition_type(const std::string& str) {
  if (!str.compare("form_plane")) {
    return ConditionType::FORM_PLANE;
  }
  if (!str.compare("less")) {
    return ConditionType::LESS;
  }

  type_error("неизвестный тип условия " + str);
}

DataType get_data_type(const std::string& str) {
  if (!str.compare("text")) {
    return DataType::TEXT;
  }
  if (!str.compare("regex")) {
    return DataType::REGEX;
  }
  if (!str.compare("var")) {
    return DataType::VAR;
  }

  type_error("неизвестный тип данных: " + str); 
}

DataObject::DataObject(
  DataType type,
  const std::string& value,
  const std::string& name
) : type(type), value(value), name(name) {
}

DataType DataObject::get_type() const { return type; }

std::string DataObject::get_value() const { return value; }

std::string DataObject::get_name() const { return name; }

ConditionObject::ConditionObject(
  const std::string& type,
  const std::vector<std::string>& names
) : ConditionObject(get_condition_type(type), names) {
}

ConditionObject::ConditionObject(
  ConditionType type,
  const std::vector<std::string>& names
) : type(type), names(names) {
}

ConditionType ConditionObject::get_type() const { return type; }

std::vector<std::string> ConditionObject::get_names() const { return names; }

double to_num(const std::string& str) {
  double val;
  try {
    val = std::stod(str);
  } catch (std::invalid_argument e) {
    throw std::runtime_error("Condition: " + str + " не получилось привести к числу");
  }
  return val;
}

Point3D::Point3D(const std::string& x_str, const std::string& y_str, const std::string& z_str) :
  x(to_num(x_str)),
  y(to_num(y_str)),
  z(to_num(z_str)) {
}

Point3D::Point3D(const double x, const double y, const double z) :
  x(x),
  y(y),
  z(z) {
}

Point3D::Point3D() :
  x(0),
  y(0),
  z(0) {
}

Vector3D::Vector3D(const Point3D& point) :
  x(point.x),
  y(point.y),
  z(point.z) {
}

Vector3D::Vector3D(const Point3D& first, const Point3D& second) :
  x(second.x - first.x),
  y(second.y - first.y),
  z(second.z - first.z) {
}

Vector3D Vector3D::cross_product(const Vector3D& other) const {
  return { {
    this->y * other.z - this->z * other.y,
    this->z * other.x - this->x * other.z,
    this->x * other.y - this->y * other.x
  } };
}

Vector3D Vector3D::operator-(const Vector3D& other) const {
  return { {
    this->x - other.x,
    this->y - other.y,
    this->z - other.z
  } };
}

Point3D Vector3D::as_point() const {
  return { this->x, this->y, this->z };
}

double Vector3D::length() const {
  return sqrt(x * x + y * y + z * z);
}