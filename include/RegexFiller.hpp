#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>

namespace RegexFiller {
     
    std::vector<std::string> regex_to_string(const std::string& pattern, unsigned int n);

}