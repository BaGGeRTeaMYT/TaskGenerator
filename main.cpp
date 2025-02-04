#include <iostream>
#include <TaskProcessor.hpp>
#include <Consumer.hpp>

int main() {
    TaskProcessor tmp(std::string(PROJECT_SOURCE_DIR) + std::string("/config.txt"));
    Consumer cons(tmp, 50);
    
    cons.write_to_file(std::string(PROJECT_SOURCE_DIR) + std::string("/tmp.tex"));

    return 0;
}