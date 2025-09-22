cmake_minimum_required(VERSION 3.12)
project(Lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Dependencies/include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

option(USE_WINAPI "Use winapi for threading" ON)
option(USE_STD_THREAD "Use thread (std::thread) for threading" OFF)

if(USE_STD_THREAD)
	set(USE_WINAPI OFF)
elseif(NOT USE_WINAPI)
	if(WIN32)
		set(USE_WINAPI ON)
	else()
		set(USE_STD_THREAD ON)
	endif()
endif()

if(USE_WINAPI)
	add_compile_definitions(USE_WINAPI)
	message(STATUS "Using winapi for threading")
elseif(USE_STD_THREAD)
	add_compile_definitions(USE_STD_THREAD)
	find_package(Threads REQUIRED)
	message(STATUS "Using thread (std::thread) for threading")
endif()

add_executable(Main Main/Main.cpp)
add_executable(Tests Tests/Tests.cpp)

if(USE_WINAPI)
	target_link_libraries(Main PRIVATE kernel32)
elseif(USE_STD_THREAD)
	target_link_libraries(Main PRIVATE Threads::Threads)
endif()

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain)