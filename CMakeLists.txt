cmake_minimum_required(VERSION 3.10)
project(particle-simulator C CXX)

set(CMAKE_C_STANDARD 11)
# enabling compile_command.json so my lsp can read the imgui code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

add_subdirectory(engine)
add_subdirectory(renderer)
add_subdirectory(app)
