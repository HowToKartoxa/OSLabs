cmake_minimum_required(VERSION 3.12)
project(Lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Dependencies/include)

add_executable(Creator Creator/Creator.cpp)
add_executable(Reporter Reporter/Reporter.cpp)
add_executable(Main Main/Main.cpp)

target_link_libraries(Main kernel32)
target_link_libraries(Reporter kernel32)
target_link_libraries(Creator kernel32)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  Tests
  Tests/Test.cpp
)
target_link_libraries(
  Tests
  PRIVATE
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Tests)



