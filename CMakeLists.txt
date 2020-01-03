cmake_minimum_required(VERSION 3.10)

# set the project name
project(sciuter VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# define sources and include directories
list(APPEND SOURCES src/main.cpp src/animation.cpp src/sdl.cpp src/systems.cpp)
list(APPEND INCLUDES "${PROJECT_SOURCE_DIR}/include")

# add the executable
add_executable(sciuter ${SOURCES})

configure_file(sciuter_config.hpp.in include/sciuter/sciuter_config.hpp)

target_include_directories(sciuter PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${INCLUDES}"
                           )

find_library(SDL2 REQUIRED)
find_library(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_iamge_INCLUDE_DIRS})
target_link_libraries(sciuter SDL2 SDL2_image)
