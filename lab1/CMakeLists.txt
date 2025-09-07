cmake_minimum_required(VERSION 3.10..3.24)

project(lab1)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
	add_executable(Main Main/main.cpp)
	add_executable(Creator Creator/creator.cpp)
	add_executable(Reporter Reporter/reporter.cpp)
else()
	message("Cannot build for this OS. Project is incompatible with this one, sorry...")
endif()



