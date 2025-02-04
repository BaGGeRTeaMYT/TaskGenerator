#include <iostream>
#include <TaskProcessor.hpp>
#include <Generator.hpp>

int main() {
    TaskProcessor tmp(std::string(PROJECT_SOURCE_DIR) + std::string("/config.txt"));
    Generator cons(tmp, 33);
    
    cons.write_to_file(std::string(PROJECT_SOURCE_DIR) + std::string("/tmp.tex"));

    return 0;
}