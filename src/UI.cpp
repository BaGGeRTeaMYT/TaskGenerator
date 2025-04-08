#include <UI.hpp>

UIWindow::UIWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("Unable to initialize glfw");
  }

  auto* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
  screen.width = videoMode->width;
  screen.height = videoMode->height;

  window = glfwCreateWindow(screen.width, screen.height, "Генератор задач", nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    throw std::runtime_error("Unable to create window");
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // TODO: Add Linux support
  io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 20, nullptr, io.Fonts->GetGlyphRangesCyrillic());

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  clear_color = glm::vec4(0.5, 0.5, 0.5, 1);
}

void UIWindow::loop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSizeConstraints(
      {400, 300},
      {static_cast<float>(screen.width), static_cast<float>(screen.height)}
    );
    ImGui::Begin("Генератор задач");

    ImGui::Text("Конфигурация: %s", app_state.config_path.c_str());

    ImGui::SameLine();
    if (ImGui::Button("Выбрать config")) {
      std::string path = select_json();
      if (!path.empty()) {
        app_state.config_path = path;
      }
    }

    float width = std::max(100.f, ImGui::GetContentRegionAvail().x * 0.3f);
    ImGui::SetNextItemWidth(width);
    ImGui::InputInt("Количество вариантов", &app_state.variantCount, 1, 10);

    ImGui::Text("Вывод: %s", app_state.output_path.c_str());

    ImGui::SameLine();
    if (ImGui::Button("Выбрать файл")) {
      std::string path = select_tex();
      if (!path.empty()) {
        app_state.output_path = path;
      }
    }

    if (ImGui::Button("Сгенерировать")) {
      if (app_state.config_path.empty()) {
        app_state.error_msg = "Укажите путь к конфигу";
      } else if (app_state.output_path.empty()) {
        app_state.error_msg = "Укажите путь вывода программы";
      } else {
        parser = std::make_shared<ParserJSON>(ParserJSON(app_state.config_path));
        try {
          parser->generate_to_file(app_state.variantCount, app_state.output_path);
          app_state.error_msg = "";
        } catch(std::runtime_error e) {
          app_state.error_msg = e.what();
        }
      }
    }

    ImGui::Text("%s", app_state.error_msg.c_str());

    ImGui::End();

    ImGui::Render();
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
}

std::string UIWindow::select_json() {
  const nfdfilteritem_t filter[] = { {"JSON", "json"} };
  return select_file(filter);
}

std::string UIWindow::select_tex() {
  const nfdfilteritem_t filter[] = { {"TEX", "tex"} };
  return select_file(filter);
}

std::string UIWindow::select_file(const nfdfilteritem_t filter[]) {
  NFD_Init();
  nfdchar_t* outPath = nullptr;
  if (NFD_OpenDialog(&outPath, filter, 1, nullptr) == NFD_OKAY) {
    std::string path = outPath;
    NFD_FreePath(outPath);
    NFD_Quit();
    return path;
  }

  NFD_Quit();
  return "";
}