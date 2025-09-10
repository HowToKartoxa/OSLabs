cmake_minimum_required(VERSION 3.12)
project(Lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Dependencies/include)


add_executable(Creator Creator/Creator.cpp)
add_executable(Reporter Reporter/Reporter.cpp)
add_executable(Main Main/Main.cpp)

target_link_libraries(Main kernel32)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)



