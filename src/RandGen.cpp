#include <RandGen.hpp>

RegexStringGenerator::RegexStringGenerator() : rng(std::random_device{}()) {}

std::string RegexStringGenerator::generate(const std::string& regex) {
  size_t pos = 0;
  return parse_expression(regex, pos);
}

char RegexStringGenerator::get_random_char(char from, char to) {
  std::uniform_int_distribution<short> dist(from, to);
  return static_cast<char>(dist(rng));
}

std::string RegexStringGenerator::parse_expression(const std::string& regex, size_t& pos) {
  std::string result;
  while (pos < regex.size()) {
    char c = regex[pos];

    if (c == '\\') {
      if (pos + 1 >= regex.size()) throw std::runtime_error("Invalid escape sequence");
      result += regex[pos + 1];
      pos += 2;
    }
    else if (c == '[') {
      size_t start = pos;
      std::string char_class = parse_character_class(regex, pos);

      if (pos < regex.size() && regex[pos] == '{') {
        std::string pattern = regex.substr(start, pos - start);
        result += parse_repetition(regex, pos, pattern);
      }
      else {
        result += char_class;
      }
    }
    else if (c == '{') {
      if (result.empty()) throw std::runtime_error("Nothing to repeat before {}");

      size_t pattern_start = pos - 1;
      std::string to_repeat = regex.substr(pattern_start, 1);
      result.pop_back();

      result += parse_repetition(regex, pos, to_repeat);
    }
    else if (c == '?') {
      if (!result.empty() && (pos == regex.size() - 1 || regex[pos + 1] != '?')) {
        if (get_random_char(0, 1)) {
          pos++;
        }
        else {
          result.pop_back();
          pos++;
        }
      }
      else {
        result += c;
        pos++;
      }
    }
    else {
      result += c;
      pos++;
    }
  }
  return result;
}

std::string RegexStringGenerator::parse_character_class(const std::string& regex, size_t& pos) {
  std::string chars;
  pos++; // Пропускаем '['

  bool in_range = false;
  char range_start = 0;

  while (pos < regex.size() && regex[pos] != ']') {
    char c = regex[pos];

    if (c == '\\') {
      if (pos + 1 >= regex.size()) throw std::runtime_error("Invalid escape sequence in character class");
      chars += regex[pos + 1];
      pos += 2;
    }
    else if (c == '-' && !chars.empty() && pos + 1 < regex.size() && regex[pos + 1] != ']') {
      in_range = true;
      range_start = chars.back();
      pos++;
    }
    else {
      if (in_range) {
        char end = c;
        for (char ch = range_start + 1; ch <= end; ch++) {
          chars += ch;
        }
        in_range = false;
        pos++;
      }
      else {
        chars += c;
        pos++;
      }
    }
  }

  if (pos >= regex.size() || regex[pos] != ']') {
    throw std::runtime_error("Unclosed character class");
  }
  pos++;

  if (chars.empty()) {
    return "";
  }

  std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);
  return std::string(1, chars[dist(rng)]);
}

std::string RegexStringGenerator::parse_repetition(const std::string& regex, size_t& pos, const std::string& pattern) {
  pos++; // Пропускаем '{'
  std::string num_str;
  bool in_range = false;
  int min_repeat = 0, max_repeat = 0;

  while (pos < regex.size() && regex[pos] != '}') {
    char c = regex[pos];

    if (c == '-') {
      if (in_range || num_str.empty()) throw std::runtime_error("Invalid repetition range");
      in_range = true;
      min_repeat = std::stoi(num_str);
      num_str.clear();
      pos++;
    }
    else if (isdigit(c)) {
      num_str += c;
      pos++;
    }
    else {
      throw std::runtime_error("Invalid character in repetition");
    }
  }

  if (pos >= regex.size() || regex[pos] != '}') {
    throw std::runtime_error("Unclosed repetition");
  }
  pos++;

  if (in_range) {
    max_repeat = num_str.empty() ? min_repeat : std::stoi(num_str);
  }
  else {
    min_repeat = max_repeat = num_str.empty() ? 0 : std::stoi(num_str);
  }

  if (min_repeat > max_repeat) {
    std::swap(min_repeat, max_repeat);
  }

  std::uniform_int_distribution<int> dist(min_repeat, max_repeat);
  int repeat_count = dist(rng);

  std::string result;
  for (int i = 0; i < repeat_count; i++) {
    size_t temp_pos = 0;
    if (pattern[0] == '[') {
      std::string full_pattern = pattern + "]";
      result += parse_character_class(full_pattern, temp_pos);
    }
    else {
      result += pattern;
    }
  }
  return result;
}