cmake_minimum_required(VERSION 3.5)
project(ParticleX  LANGUAGES CXX)
set(BUILD_SHARED_LIBS FALSE)

# SFML
# ----------------------------------------------
#include(FetchContent)
#FetchContent_Declare(SFML
#    GIT_REPOSITORY https://github.com/SFML/SFML.git
#    GIT_TAG 3.0.0
#    GIT_SHALLOW ON
#    SYSTEM)
#FetchContent_MakeAvailable(SFML)


# Set standarts
# ----------------------------------------------

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(./include)

include(./src/CMakeLists.txt)