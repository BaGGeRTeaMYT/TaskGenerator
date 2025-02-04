#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <stdexcept>
#include <RegexFiller.hpp>

class TaskProcessor {
public:
    TaskProcessor(const std::string& file_name);

    struct TokenType {
        TokenType(const std::string& b, bool is_r): buf(b), is_regex(is_r) {}
        std::string buf;
        bool is_regex;
    };
    using TokenVec = std::vector<TokenType>;

    const TokenVec& get_token_vec() const;

    std::vector<std::string> fill_regex(unsigned int result_size) const;

private:
    TokenVec read_from_file(std::ifstream& file);

    TokenVec m_str;
};