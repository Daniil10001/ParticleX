

add_executable(main ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(main PRIVATE Threads::Threads)