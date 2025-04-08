#include <iostream>
#include <ParserJSON.hpp>
#include <RandGen.hpp>
#include <fstream>
#include <chrono>
#include <UI.hpp>

int main() {
  UIWindow ui;
  std::ofstream output("D:/TaskGenerator/main_out.txt");
  ParserJSON js("D:/TaskGenerator/test_config.json");
  RegexStringGenerator generator;

  auto start = std::chrono::high_resolution_clock::now();

  auto end = std::chrono::high_resolution_clock::now();

  std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(end - start)).count() / 1000. << " seconds elapsed" << std::endl;
  ui.loop();
  return 0;
}