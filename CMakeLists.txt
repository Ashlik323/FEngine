cmake_minimum_required(VERSION 3.24)

include(CMakePrintHelpers)

add_subdirectory("includes/glfw-3.4")

project(FlagmanEngine)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

cmake_print_variables(MY_SOURCES)

add_executable(${PROJECT_NAME} "${MY_SOURCES}" "includes/glad/src/glad.c")

target_include_directories(${PROJECT_NAME} PUBLIC "src/" PUBLIC "includes/")

target_link_libraries(${PROJECT_NAME} glfw)