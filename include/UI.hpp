#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <nfd.h>

#include <ParserJSON.hpp>
#include <algorithm>

struct AppState {
  std::string config_path = "";
  std::string output_path = "";
  std::string msg  = "";
  int variantCount = 1;
  bool generatePressed = false;
};

struct Rectangle {
  int width;
  int height;
};

class UIWindow {
public:
  UIWindow();

  void loop();

private:
  AppState app_state;
  GLFWwindow* window;
  glm::vec4 clear_color;
  std::shared_ptr<ParserJSON> parser;

  Rectangle screen;

  std::string select_json();
  std::string select_tex();
  std::string select_file(const nfdfilteritem_t filter[]);
};