cmake_minimum_required(VERSION 3.24)

project(TaskGenerator LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_Declare(
    imgui-glfw-glad-glm
    GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
    GIT_TAG master
)

FetchContent_Declare(
    nfd
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
    GIT_TAG master
)

FetchContent_MakeAvailable(nlohmann_json)
FetchContent_MakeAvailable(imgui-glfw-glad-glm)
FetchContent_MakeAvailable(nfd)

if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenGL REQUIRED opengl)
else()
    find_package(OpenGL REQUIRED)
endif()

file (GLOB SOURCES "src/*.cpp")

add_compile_definitions(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

include_directories(include)

add_executable(MAIN main.cpp ${SOURCES})

target_link_libraries(MAIN PRIVATE
    nlohmann_json::nlohmann_json
    imgui
    glm
    OpenGL::GL
    nfd)