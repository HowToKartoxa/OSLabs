cmake_minimum_required(VERSION 3.12)
project(Lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(Dependencies/include)


add_executable(Creator Creator/Creator.cpp)
add_executable(Reporter Reporter/Reporter.cpp)
add_executable(Main Main/Main.cpp)

target_link_libraries(Main kernel32)
target_link_libraries(Reporter kernel32)
target_link_libraries(Creator kernel32)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

enable_testing()

add_executable(
  Tests
  Tests/Test.cpp
)
target_link_libraries(
  Tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Tests)



