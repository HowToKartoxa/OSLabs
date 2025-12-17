cmake_minimum_required(VERSION 3.12)
project(Lab5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(Dependencies/include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

option(USE_WINAPI "Use winapi for server and client" OFF)
option(USE_BOOST "Use Boost libraries for server and client" OFF)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_ARCHITECTURE "-x64")

find_package(Boost REQUIRED COMPONENTS thread unit_test_framework date_time)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(COMMON_SOURCES
    Dependencies/src/input_parsing.cpp
	Dependencies/src/employee_database_internal.cpp
	Dependencies/src/employee_database.cpp
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
	message(STATUS "Using winapi for server and client")
	set(PLATFORM_SOURCES
		Dependencies/src/server.cpp
		Dependencies/src/client.cpp
	)
elseif(USE_BOOST)
	add_compile_definitions(USE_BOOST)
	message(STATUS "Using Boost libraries for server and client")
	set(PLATFORM_SOURCES
		Dependencies/src/server_boost.cpp
		Dependencies/src/client_boost.cpp
	)
endif()

add_executable(Server Server/Server_proc.cpp ${COMMON_SOURCES} ${PLATFORM_SOURCES})
add_executable(Client Client/Client_proc.cpp ${COMMON_SOURCES} ${PLATFORM_SOURCES})

add_executable(Tests Tests/Tests.cpp ${COMMON_SOURCES})

target_link_libraries(Tests PRIVATE Boost::unit_test_framework)

if(USE_WINAPI)
	target_link_libraries(Server PRIVATE kernel32)
	target_link_libraries(Client PRIVATE kernel32)
	target_link_libraries(Tests PRIVATE kernel32)
elseif(USE_BOOST)
	target_link_libraries(Server PRIVATE Boost::thread)
	target_link_libraries(Client PRIVATE Boost::thread)
	target_link_libraries(Tests PRIVATE Boost::thread)
	target_link_libraries(Server PRIVATE Boost::date_time)
	target_link_libraries(Client PRIVATE Boost::date_time)
	if(WIN32)
		target_link_libraries(Client PRIVATE ws2_32 ntdll)
		target_link_libraries(Server PRIVATE ws2_32 ntdll)
	endif()
endif()