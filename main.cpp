#include <iostream>
#include <fstream>
#include <cstdlib> // Для функции system()
#include <windows.h> // Для функции chdir()
#include <string>
#include <TaskProcessor.hpp>

int main() {
    TaskProcessor tmp("D:/AlgebraTaskGen/config.txt");

    std::cout << tmp.fill_regex();

    return 0;
}