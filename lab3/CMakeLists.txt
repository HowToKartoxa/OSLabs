cmake_minimum_required(VERSION 3.12)
project(Lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Dependencies/include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

option(USE_WINAPI "Use winapi for threading" OFF)
option(USE_BOOST "Use Boost libraries for threading" OFF)

if(USE_BOOST)
	set(USE_WINAPI OFF)
elseif(NOT USE_WINAPI)
	if(WIN32)
		set(USE_WINAPI ON)
	else()
		set(USE_BOOST ON)
	endif()
endif()

if(USE_WINAPI)
	add_compile_definitions(USE_WINAPI)
	message(STATUS "Using winapi for threading")
elseif(USE_BOOST)
	add_compile_definitions(USE_BOOST)
	message(STATUS "Using Boost libraries for threading")
endif()

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Main Main/Main.cpp)

if(USE_WINAPI)
	target_link_libraries(Main PRIVATE kernel32)
elseif(USE_STD_THREAD)
	target_link_libraries(Main PRIVATE Boost::thread)
	# and others
endif()

add_executable(Tests Tests/Tests.cpp)

target_link_libraries(Tests PRIVATE Boost::test)