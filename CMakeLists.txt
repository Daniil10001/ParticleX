cmake_minimum_required(VERSION 3.5)
project( Factorio  LANGUAGES CXX)

if (NOT DLEVEL)
    set(DLEVEL 10)
endif()

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
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -g -DDLEVEL=${DLEVEL})
include_directories(./include)


include(./src/CMakeLists.txt)