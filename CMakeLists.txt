cmake_minimum_required(VERSION 3.10)

project(fabrica VERSION 0.0.1)

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BINARY_DIR})

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(fabrica
    fabrica/error.c
    fabrica/main.c

    fabrica/math/mat4f.c
    fabrica/math/quaternionf.c
    fabrica/math/vec3f.c

    fabrica/renderer/chunk_mesh.c
    fabrica/renderer/voxel_renderer.c

    fabrica/world/chunk.c

    stb/stb_image.c
)
target_include_directories(fabrica PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_features(fabrica PRIVATE c_std_11)
target_link_libraries(fabrica PRIVATE 
    glad::glad
    glfw
)
