cmake_minimum_required(VERSION 3.12)
project(Lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(Dependencies/include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

option(USE_WINAPI "Use winapi for message queue" OFF)
option(USE_BOOST "Use Boost libraries for message queue" OFF)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_ARCHITECTURE "-x64")

find_package(Boost REQUIRED COMPONENTS unit_test_framework date_time)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(COMMON_SOURCES
    Dependencies/src/input_parsing.cpp
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
	message(STATUS "Using winapi for message queue")
	set(PLATFORM_SOURCES
		Dependencies/src/message_queue.cpp
	)
elseif(USE_BOOST)
	add_compile_definitions(USE_BOOST)
	message(STATUS "Using Boost libraries for message queue")
	set(PLATFORM_SOURCES
		Dependencies/src/message_queue_boost.cpp
	)
endif()

add_executable(Sender Sender/Sender.cpp ${COMMON_SOURCES} ${PLATFORM_SOURCES})
add_executable(Receiver Receiver/Receiver.cpp ${COMMON_SOURCES} ${PLATFORM_SOURCES})

add_executable(Tests Tests/Tests.cpp ${COMMON_SOURCES} ${PLATFORM_SOURCES})

target_link_libraries(Tests PRIVATE Boost::unit_test_framework)
target_link_libraries(Sender PRIVATE kernel32)
target_link_libraries(Receiver PRIVATE kernel32)
target_link_libraries(Tests PRIVATE kernel32)

if(USE_BOOST)
	target_link_libraries(Sender PRIVATE Boost::date_time)
	target_link_libraries(Receiver PRIVATE Boost::date_time)
	target_link_libraries(Tests PRIVATE Boost::date_time)
endif()