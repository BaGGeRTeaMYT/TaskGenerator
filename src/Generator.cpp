#include <Generator.hpp>

Generator::Generator(const TaskProcessor& proc, unsigned int n) :
  m_processor(std::make_unique<TaskProcessor>(proc)),
  m_amount_of_tasks(n) {
}

void Generator::set_amount(unsigned int new_amount) {
  m_amount_of_tasks = new_amount;
}

// TODO: check for repeating tasks
// TODO: add user to allow some amount of repeatitions
void Generator::generate_tasks() {
  m_last_generated = m_processor->fill_regex(m_amount_of_tasks);
}

void Generator::write_to_file(const std::string& file_name) {

  std::ofstream file(file_name);
  if (!file.is_open()) {
    std::string error_msg = "Unable to open file " + file_name;
    throw std::runtime_error(error_msg);
  }

  if (m_last_generated.size() != m_amount_of_tasks) {
    generate_tasks();
  }

  file << "\\documentclass[11pt]{article}\n"
    << "\\usepackage{amsmath, amsfonts, amsthm}\n\\usepackage{nicefrac}\n"
    << "\\usepackage[utf8]{inputenc}\n\\usepackage[T2A]{fontenc}\n\\usepackage[russian]{babel}\n"
    << "\\usepackage{fontspec}\n\\setmainfont{Times New Roman}\n"
    << "\\begin{document}\n";

  for (int i = 0; i < m_last_generated.size(); i++) {
    file << "\\subsection*{Вариант " << i + 1 << "}\n"
      << m_last_generated[i] << "\n";
  }
  file << "\\end{document}\n";

}

