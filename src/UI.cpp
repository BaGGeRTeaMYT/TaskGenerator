#include <UI.hpp>

UIWindow::UIWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("Unable to initialize glfw");
  }

  auto* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
  screen.width = videoMode->width;
  screen.height = videoMode->height;

  window = glfwCreateWindow(screen.width / 3, screen.height / 5, "Генератор задач", nullptr, nullptr);
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

  clear_color = glm::vec4(0.1, 0.1, 0.1, 1);
}

void UIWindow::loop() {
  if (!app_state.config_path.empty()) {
    parser = std::make_shared<ParserJSON>(ParserJSON(app_state.config_path));
  }
  if (!app_state.output_path.empty()) {
    app_state.solution_path = app_state.output_path.substr(0, app_state.output_path.find_last_of('.')) + "_solution.ipynb";
  }
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("Генератор задач", nullptr, flags);
    ImGui::PopStyleVar(3);

    ImGui::Text("Конфигурация: %s", app_state.config_path.c_str());

    ImGui::SameLine();
    if (ImGui::Button("Выбрать config")) {
      std::string path = select_json();
      if (!path.empty()) {
        app_state.config_path = path;
        parser = std::make_shared<ParserJSON>(ParserJSON(app_state.config_path));
      }
    }


    ImGui::SameLine();
    if (ImGui::Button("Редактировать config")) {
      open_file(app_state.config_path);
    }

    ImGui::SetNextItemWidth(200);
    ImGui::InputInt("Количество вариантов", &app_state.variantCount, 1, 10);

    ImGui::Text("Вывод: %s", app_state.output_path.c_str());

    ImGui::SameLine();
    if (ImGui::Button("Выбрать файл")) {
      std::string path = select_tex();
      if (!path.empty()) {
        app_state.output_path = path;
        app_state.solution_path = app_state.output_path.substr(0, app_state.output_path.find_last_of('.')) + "_solution.ipynb";
      }
    }

    ImGui::SameLine();
    if (ImGui::Button("Открыть файл")) {
      open_file(app_state.output_path);
    }

    if (parser && parser->has_solutions()) {
      ImGui::Text("Ответы: %s", app_state.solution_path.c_str());
      ImGui::SameLine();
      if (ImGui::Button("Выбрать файл##2")) {
        std::string path = select_ipynb();
        if (!path.empty()) {
          app_state.solution_path = path;
        }
      }

    }

    if (ImGui::Button("Сгенерировать")) {
      if (app_state.config_path.empty()) {
        app_state.msg = "Укажите путь к конфигу";
        app_state.holding_error_message = true;
      }
      else if (app_state.output_path.empty()) {
        app_state.msg = "Укажите путь вывода программы";
        app_state.holding_error_message = true;
      }
      else {
        try {
          parser->generate_to_file(
            app_state.variantCount,
            app_state.output_path,
            app_state.solution_path
          );
          app_state.msg = "Генерация успешно завершилась";
          app_state.holding_error_message = false;
        }
        catch (std::runtime_error e) {
          app_state.msg = e.what();
          app_state.holding_error_message = true;
        }
      }
    }

    ImVec4 message_color = (app_state.holding_error_message ? ImVec4(1, 0, 0, 1) : ImVec4(1, 1, 1, 1));

    ImGui::TextColored(message_color, "%s", app_state.msg.c_str());

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

std::string UIWindow::select_ipynb() {
  const nfdfilteritem_t filter[] = { {"IPYNB", "ipynb"} };
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

void UIWindow::open_file(const std::string& filePath) {
#ifdef _WIN32
  std::string command = "start \"\" \"" + filePath + "\"";
#elif __APPLE__
  std::string command = "open \"" + filePath + "\"";
#else // Linux
  std::string command = "xdg-open \"" + filePath + "\"";
#endif

  std::system(command.c_str());
}