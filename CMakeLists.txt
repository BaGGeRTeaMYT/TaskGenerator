cmake_minimum_required(VERSION 3.24)

project(TaskGenerator)

add_compile_definitions(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

include_directories(include)

add_executable(MAIN main.cpp src/TaskProcessor.cpp src/RegexFiller.cpp src/Generator.cpp)