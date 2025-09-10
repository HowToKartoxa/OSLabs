cmake_minimum_required(VERSION 3.12)
project(Lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/Dependencies/include/utils)

add_executable(Creator Creator/Creator.cpp)
target_link_libraries(Creator user32 gdi32 kernel32 advapi32)

add_executable(Reporter Reporter/Reporter.cpp)
target_link_libraries(Reporter user32 gdi32 kernel32 advapi32)

add_executable(Main Main/Main.cpp)
target_link_libraries(Main user32 gdi32 kernel32 advapi32)



