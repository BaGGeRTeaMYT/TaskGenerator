#include <ConditionChecker.hpp>

bool form_plane(const std::map<std::string, std::string>& var_values, const std::vector<std::string>& names) {
  if (names.size() != 9) {
    std::string error_msg = "Can only construct plane from 9 names (" + std::to_string(names.size()) + " given)";
    throw std::runtime_error(error_msg);
  }
  std::array<Point3D, 3> plane = {};
  for (int j = 0; j < plane.size(); j++) {
    int i = j*3;
    plane[j] = {var_values.at(names[i]), var_values.at(names[i + 1]), var_values.at(names[i + 2])};
  }
  Vector3D first(plane[0], plane[1]);
  Vector3D second(plane[0], plane[2]);
  Vector3D cross_prod = first.cross_product(second);

  return cross_prod.length() > std::numeric_limits<double>::epsilon();
}

bool less(const std::map<std::string, std::string>& var_values, const std::vector<std::string>& names) {
  if (names.size() != 2) {
      throw std::runtime_error("\"less\" ожидает 2 аргумента, получено " + std::to_string(names.size()));
  } 
  std::array<double, 2> arr;
  for (int i = 0; i < names.size(); i++) {
    try {
      arr[i] = std::stod(var_values.at(names[i]));
    } catch (std::invalid_argument e) {
      throw std::runtime_error(names[i] + ": " + var_values.at(names[i]) + " не получилось привести к числу");
    }
  }
  return arr[0] < arr[1];
}

ConditionChecker::ConditionChecker() {
  checkers[ConditionType::FORM_PLANE] = form_plane;
  checkers[ConditionType::LESS] = less;
}

ConditionChecker::CheckerFunction ConditionChecker::operator[](ConditionType type) const {
  auto iter = checkers.find(type);
  if (iter == checkers.end()) {
    throw std::runtime_error("Unknown condition type");
  }
  return iter->second;
}
