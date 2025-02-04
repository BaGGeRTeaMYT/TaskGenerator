#pragma once

#include <TaskProcessor.hpp>
#include <memory>

class Generator {
public:
    Generator(const TaskProcessor& processor, unsigned int amount_of_tasks);

    void set_amount(unsigned int new_amount);

    void generate_tasks();

    void write_to_file(const std::string& file_name);

private:
    unsigned int m_amount_of_tasks;
    std::unique_ptr<TaskProcessor> m_processor;
    std::vector<std::string> m_last_generated;
};