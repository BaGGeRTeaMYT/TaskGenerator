#include <iostream>
#include <ParserJSON.hpp>
#include <RandGen.hpp>
#include <fstream>

int main() {
  std::ofstream output("D:/TaskGenerator/main_out.txt");
  ParserJSON js("D:/TaskGenerator/test_config.json");
  RegexStringGenerator generator;
  
  for (int i = 0; i < 1e+3; i++) {
    js.generate_values();
    std::cout << i << std::endl;
    output << js.to_string();
  }

  return 0;
}