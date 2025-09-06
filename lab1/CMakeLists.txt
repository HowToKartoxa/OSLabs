cmake_minimum_required(VERSION 3.10)

project(lab1 CXX)

if(WIN32)
	message("building...")

	add_executable(Main Main/main.cpp)
	add_executable(Creator Creator/creator.cpp)
	add_executable(Reporter Reporter/reporter.cpp)
else()
	message("Cannot build for this OS. Project is incompatible with this one, sorry...")
endif()



