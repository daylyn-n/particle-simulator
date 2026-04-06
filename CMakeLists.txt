cmake_minimum_required(VERSION 3.10)
project(particle-simulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(engine)
add_subdirectory(renderer)
add_subdirectory(app)
