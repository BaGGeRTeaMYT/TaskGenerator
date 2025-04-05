#include <iostream>
#include <TaskProcessor.hpp>
#include <Generator.hpp>
#include <ParserJSON.hpp>
#include <RandGen.hpp>
// #include <fstream>

int main() {
  // TaskProcessor tmp(std::string(PROJECT_SOURCE_DIR) + std::string("/config.txt"));
  // Generator cons(tmp, 33);

  // cons.write_to_file(std::string(PROJECT_SOURCE_DIR) + std::string("/tmp.tex"));
  
  std::ofstream output("D:/TaskGenerator/main_out.txt");
  ParserJSON js("D:\\TaskGenerator\\test_config.json");
  for (const auto& i : js.get_data()) {
    output << i.get_value() << std::endl;
  }
  RegexStringGenerator generator;

  output << "Escape: " << generator.generate("\\[test\\]") << "\n";
  output << "Range: " << generator.generate("[1-2][0-9]") << "\n";
  output << "Repetition: " << generator.generate("[a-z]{3}") << "\n";
  output << "Optional: " << generator.generate("a?b?c?") << "\n";
  output << "Variable repeat: " << generator.generate("1{0-2}") << "\n";

  return 0;
}