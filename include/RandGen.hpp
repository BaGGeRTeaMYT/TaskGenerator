#pragma once

#include <string>
#include <random>
#include <stdexcept>
#include <algorithm>

class RegexStringGenerator {
public:
  RegexStringGenerator();
  std::string generate(const std::string& regex);

private:
  std::mt19937 rng;

  char get_random_char(char from, char to);
  std::string parse_expression(const std::string& regex, size_t& pos);
  std::string parse_character_class(const std::string& regex, size_t& pos);
  std::string parse_repetition(const std::string& regex, size_t& pos, const std::string& to_repeat);
};