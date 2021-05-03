if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DAE_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DAE_RELEASE)
endif()

option(BUILD_TESTS "Build unit tests." ON)