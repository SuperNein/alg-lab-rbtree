cmake_minimum_required(VERSION 3.16)
project(rb_tree_dict LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# default: Debug
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type: Debug | Release | RelWithDebInfo" FORCE)
endif()

option(ENABLE_GPROF "Enable gprof profiling flags (-pg -g)" OFF)

add_library(compiler_warnings INTERFACE)
target_compile_options(compiler_warnings INTERFACE -Wall -Wextra -Wpedantic -Werror)

add_library(rb_tree STATIC
    src/rb_tree.cpp
    src/rb_tree_io.cpp
)
target_include_directories(rb_tree PUBLIC include)
target_link_libraries(rb_tree PRIVATE compiler_warnings)

add_executable(dict src/main.cpp)
target_link_libraries(dict PRIVATE rb_tree compiler_warnings)

if(ENABLE_GPROF)
    target_compile_options(rb_tree PRIVATE -pg -g)
    target_compile_options(dict PRIVATE -pg -g)
    target_link_options(dict PRIVATE -pg)
endif()
