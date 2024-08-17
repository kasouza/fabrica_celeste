cmake_minimum_required(VERSION 3.10)

project(mine VERSION 0.0.1)

find_package(c_std_11 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(mine 
    mine/main.cpp
)
target_include_directories(mine PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_features(mine PRIVATE c_std_11)
target_link_libraries(mine PRIVATE 
    glad::glad
    glfw
)
