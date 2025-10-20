cmake_minimum_required(VERSION 3.12)
project(Lab3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(Dependencies/include)
link_directories(Dependencies/include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

option(USE_WINAPI "Use winapi for threading" OFF)
option(USE_BOOST "Use Boost libraries for threading" OFF)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_ARCHITECTURE "-x64")

find_package(Boost REQUIRED COMPONENTS unit_test_framework thread)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(COMMON_SOURCES
    Dependencies/src/input_parsing.cpp
	Dependencies/src/marker_procedures.cpp
	Dependencies/src/event_boost.cpp
)

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
	set(PLATFORM_SOURCES
		Dependencies/src/thread_manager.cpp
	)
elseif(USE_BOOST)
	add_compile_definitions(USE_BOOST)
	message(STATUS "Using Boost libraries for threading")
	set(PLATFORM_SOURCES
		Dependencies/src/thread_manager_boost.cpp
	)
endif()

add_executable(Main Main/Main.cpp ${COMMON_SOURCES} ${PLATFORM_SOURCES})

add_executable(Tests Tests/Tests.cpp ${COMMON_SOURCES} ${PLATFORM_SOURCES})

target_link_libraries(Tests PRIVATE Boost::unit_test_framework)

if(USE_WINAPI)
	target_link_libraries(Main PRIVATE kernel32)
	target_link_libraries(Tests PRIVATE kernel32)
elseif(USE_BOOST)
	target_link_libraries(Main PRIVATE Boost::thread)
	target_link_libraries(Tests PRIVATE Boost::thread)
endif()

add_test(NAME AllTests COMMAND Tests)