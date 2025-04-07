#include <ConditionChecker.hpp>

ConditionChecker::ConditionChecker() {
  checkers[ConditionType::FORM_PLANE] = form_plane;
}

Point3D get_point(const std::vector<DataObject>& objects, const std::vector<std::string>& names) {
  if (names.size() != 3) {
    std::string error_msg = "Can only construct point from 3 names (" + std::to_string(names.size()) + " given)";
    throw std::runtime_error(error_msg);
  }
  std::array<std::string, 3> coords = {};
  for (const auto& obj : objects) {
    if (!obj.get_name().empty()) {
      for (int i = 0; i < names.size(); i++) {
        if (!obj.get_name().compare(names[i])) {
          if (coords[i].size()) {
            std::string error_msg = "Found second object with name " + obj.get_name();
            throw std::runtime_error(error_msg);
          }
          coords[i] = obj.get_value();
          break;
        }
      }
    }
  }
  for (int i = 0; i < coords.size(); i++) {
    if (coords[i].empty()) {
      std::string error_msg = "Object with name \"" + names[i] + "\" not found"; 
      throw std::runtime_error(error_msg);
    }
  }
  return {coords[0], coords[1], coords[2]};
}

bool ConditionChecker::form_plane(const std::vector<DataObject>& objects, const std::vector<std::string>& names) {
  if (names.size() != 9) {
    std::string error_msg = "Can only construct plane from 9 names (" + std::to_string(names.size()) + " given)";
    throw std::runtime_error(error_msg);
  }
  std::array<Point3D, 3> plane = {};
  for (int j = 0; j < plane.size(); j++) {
    int i = j*3;
    plane[j] = get_point(objects, {names[i], names[i + 1], names[i + 2]});
  }
  Vector3D first(plane[0], plane[1]);
  Vector3D second(plane[0], plane[2]);
  Vector3D cross_prod = first.cross_product(second);

  return cross_prod.length() > std::numeric_limits<double>::epsilon();
}

ConditionChecker::CheckerFunction ConditionChecker::operator[](ConditionType type) const {
  auto iter = checkers.find(type);
  if (iter == checkers.end()) {
    throw std::runtime_error("Unknown condition type");
  }
  return iter->second;
}