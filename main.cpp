#include <iostream>
#include <ParserJSON.hpp>
#include <RandGen.hpp>
#include <fstream>
#include <chrono>

int main() {
  std::ofstream output("D:/TaskGenerator/main_out.txt");
  ParserJSON js("D:/TaskGenerator/test_config.json");
  RegexStringGenerator generator;

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 500; i++) {
    output << "Вариант " << i + 1 << ":\n";
    js.generate_values();
    output << js.to_string();
  }

  auto end = std::chrono::high_resolution_clock::now();

  std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(end - start)).count() << std::endl;

  return 0;
}