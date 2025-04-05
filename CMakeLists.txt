cmake_minimum_required(VERSION 3.24)

project(TaskGenerator)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

add_compile_definitions(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

include_directories(include)

add_executable(MAIN
  main.cpp
  src/TaskProcessor.cpp
  src/RegexFiller.cpp
  src/Generator.cpp
  src/ParserJSON.cpp
  src/RandGen.cpp)

target_link_libraries(MAIN PRIVATE nlohmann_json::nlohmann_json)