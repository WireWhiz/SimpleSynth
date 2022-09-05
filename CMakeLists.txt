cmake_minimum_required(VERSION 3.23)
project(SimpleSynth)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(ext)
add_subdirectory(src)

add_executable(SimpleSynth main.cpp)
target_link_libraries(SimpleSynth PUBLIC synth graphics)