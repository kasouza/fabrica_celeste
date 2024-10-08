cmake_minimum_required(VERSION 3.10)

project(fabrica VERSION 0.0.1)

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${PROJECT_BINARY_DIR})

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
  fabrica
  fabrica/error.c
  fabrica/main.c
  fabrica/file_io/read_file.c
  fabrica/file_io/image.c
  fabrica/math/mat4f.c
  fabrica/math/quaternionf.c
  fabrica/math/vec3f.c
  fabrica/renderer/camera.c
  fabrica/renderer/chunk_mesh.c
  fabrica/renderer/renderer.c
  fabrica/renderer/shaders.c
  fabrica/renderer/texture.c
  fabrica/renderer/texture_atlas.c
  fabrica/world/block.c
  fabrica/world/chunk.c
  fabrica/world/world.c
  stb/stb_image.c)

target_include_directories(fabrica PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_features(fabrica PRIVATE c_std_11)
target_link_libraries(fabrica PRIVATE glad::glad glfw)
