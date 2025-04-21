#include <UI.hpp>

UIWindow::UIWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("Fatal: не удалось инициализировать glfw");
  }

  auto* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
  screen.width = videoMode->width;
  screen.height = videoMode->height;

  window = glfwCreateWindow(screen.width / 2, screen.height / 3, "Генератор задач", nullptr, nullptr);
  
  if (window == nullptr) {
    glfwTerminate();
    throw std::runtime_error("Fatal: не удалось создать окно");
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.IniFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // TODO: Add Linux support
  io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 20, nullptr, io.Fonts->GetGlyphRangesCyrillic());

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  clear_color = glm::vec4(0.1, 0.1, 0.1, 1);
}

UIWindow::~UIWindow() {
  if (generate_thread.joinable()) {
    generate_thread.join();
  }
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

    if (generation_complete) {
      app_state.msg = generation_result;
      app_state.holding_error_message = !generation_result.empty() && 
          generation_result != "Генерация успешно завершилась";
      
      if (generate_thread.joinable()) {
          generate_thread.join();
      }
      
      generation_complete = false;
    }
    
    ImGui::Text("Конфигурация: %s", app_state.config_path.c_str());
    
    ImGui::SameLine();
    if (ImGui::Button("Выбрать config")) {
      std::string path = select_json();
      if (!path.empty()) {
        app_state.config_path = path;
        try {
          parser = std::make_shared<ParserJSON>(ParserJSON(app_state.config_path));
        } catch (std::runtime_error e) {
          app_state.holding_error_message = true;
          app_state.msg = e.what();
        }
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
      app_state.need_to_generate = true;
    }
    
    ImVec4 message_color = (app_state.holding_error_message ? ImVec4(1, 0, 0, 1) : ImVec4(1, 1, 1, 1));
    
    ImGui::TextColored(message_color, "%s", app_state.msg.c_str());
    
    if (app_state.need_to_generate && !is_generating) {
      is_generating = true;
      generation_complete = false;
      app_state.msg = "Идёт генерация...";
      
      // Запуск в отдельном потоке
      if (generate_thread.joinable()) {
          generate_thread.join();
      }
      generate_thread = std::thread(&UIWindow::generate_thread_wrapper, this);
    }

    ImGui::End();
    
    ImGui::Render();
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(window);
  }
  if (generate_thread.joinable()) {
    generate_thread.join();
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

void UIWindow::generate() {

  std::lock_guard<std::mutex> lock(generate_mutex);

  if (app_state.config_path.empty()) {
    generation_result = "Укажите путь к конфигу";
    app_state.holding_error_message = true;
    is_generating = false;
    app_state.need_to_generate = false;
    generation_complete = true;
    return;
  }
  else if (app_state.output_path.empty()) {
    generation_result = "Укажите путь для вывода программы";
    app_state.holding_error_message = true;
    is_generating = false;
    app_state.need_to_generate = false;
    generation_complete = true;
    return;
  }
  else {
    try {
      parser->generate_to_file(
        app_state.variantCount,
        app_state.output_path,
        app_state.solution_path
      );
      generation_result = "Генерация успешно завершилась";
      app_state.holding_error_message = false;
    }
    catch (std::runtime_error e) {
      generation_result = e.what();
      app_state.holding_error_message = true;
    }
  }
  generation_complete = true;
  is_generating = false;
  app_state.need_to_generate = false;
}

void UIWindow::generate_thread_wrapper() {
  generate();
}