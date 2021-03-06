cmake_minimum_required(VERSION 3.18)

project (AnvilEngine)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(AE_THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/Engine/Code/ThirdParty")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include ("CMake/CommonOptions.cmake")
include ("CMake/Modules.cmake")
include ("CMake/BuildEngine.cmake")
include ("CMake/BuildLauncher.cmake")

if(BUILD_TESTS)
	add_subdirectory("Engine/Code/Runtime/Tests")
endif()