#include <RegexFiller.hpp>

std::vector<std::string> RegexFiller::regex_to_string(const std::string& pattern, unsigned int n) {
    #ifdef PROJECT_SOURCE_DIR
        std::string project_source_dir = PROJECT_SOURCE_DIR;
    #else
        throw std::runtime_error ("PROJECT_SOURCE_DIR not defined");
    #endif

    // Полные пути к файлам и скрипту
    std::string input_file_path = project_source_dir + "/input.txt";
    std::string output_file_path = project_source_dir + "/output.txt";
    std::string python_script_path = project_source_dir + "/StringGenerator.py";

    // Запись регулярного выражения в файл
    std::ofstream input_file(input_file_path);
    if (!input_file) {
        throw std::runtime_error("Unable to open input file for writing\n");
    }
    input_file << pattern;
    input_file.close();

    std::string command = "python " + python_script_path + " " + input_file_path + " " + output_file_path + " " + std::to_string(n);
    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Error in Python code\n");
    }

    std::ifstream output_file(output_file_path);
    if (!output_file) {
        throw std::runtime_error("Unable to open output file to read\n");
    }
    std::vector<std::string> res(n);
    for (int i = 0; i < n; i++) {
        std::getline(output_file, res[i]);
    }
    output_file.close();

    return res;
}