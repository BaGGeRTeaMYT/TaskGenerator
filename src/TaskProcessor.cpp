#include <TaskProcessor.hpp>

TaskProcessor::TaskProcessor(const std::string& file_name) {
    std::ifstream file(file_name);

    if (!file.is_open()) {
        std::string error_msg = "Unable to open file " + file_name + "\n";
        throw std::runtime_error(error_msg);
    }

    m_str = read_from_file(file);
}

TaskProcessor::TokenVec TaskProcessor::read_from_file(std::ifstream& file) {
    TokenVec res;
    
    int brackets_counter = 0;
    bool reading_regex = false;
    bool prev_backslash = false;
    std::string cur_line = "";
    char cur_sym;

    // TODO: add \{ and \} support
    while (file.get(cur_sym)) {
        if (cur_sym == '\\') {
            if (!prev_backslash) {
                prev_backslash = true;
            } else {
                cur_line += "\\";
                prev_backslash = false;
            }
        } else {
            
            if (prev_backslash) {
                if (cur_sym == '{' || cur_sym == '}') {
                    if (reading_regex) {
                        cur_line.push_back('\\');
                        cur_line.push_back(cur_sym);
                    } else {
                        cur_line.push_back(cur_sym);
                    }
                } else {
                    cur_line.push_back('\\');
                    cur_line.push_back(cur_sym);
                }
            } else {
                brackets_counter += (cur_sym == '{') - (cur_sym == '}');

                if (cur_sym == '{' && !reading_regex) {
                    res.emplace_back(cur_line, reading_regex);
                    cur_line.clear();
                    reading_regex ^= 1;
                } else if (cur_sym == '}' && reading_regex && brackets_counter == 0) {
                    res.emplace_back(cur_line, reading_regex);
                    cur_line.clear();
                    reading_regex ^= 1;
                } else {
                    cur_line += cur_sym;
                }
            }
            prev_backslash = false;
        }
    }

    if (reading_regex) {
        std::string error_msg = "Unterminated regex in file\n";
        throw std::runtime_error(error_msg);
    }

    if (!cur_line.empty()) {
        res.emplace_back(cur_line, reading_regex);
    }

    return res;
}

std::vector<std::string> TaskProcessor::fill_regex(unsigned int result_size) const {
    std::vector<std::string> res(result_size);
    for (auto token : m_str) {
        if (token.is_regex) {
            std::vector<std::string> regex_res = RegexFiller::regex_to_string(token.buf, result_size);
            for (int i = 0; i < res.size(); i++) {
                res[i] += regex_res[i];
            }
        } else {
            for (int i = 0; i < res.size(); i++) {
                res[i] += token.buf;
            }
        }
    }
    return res;
}