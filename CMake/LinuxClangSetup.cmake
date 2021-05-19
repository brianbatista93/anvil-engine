set(BUILD_CPU_ARCHITECTURE x64)
set(BUILD_PLATFORM Linux64)
set(LINUX TRUE)
set(PLATFORM_NAME "Linux")

set(CMAKE_CONFIGURATION_TYPES Debug Profile Release)
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)

add_definitions(-DAE_LINUX)

if(NOT CMAKE_C_COMPILER)
	set(CMAKE_C_COMPILER clang)
endif()

if(NOT CMAKE_CXX_COMPILER)
	set(CMAKE_CXX_COMPILER clang++)
endif()

set(CLANG_COMMON_FLAGS
	-Werror
	-Wall
)

if(ANDROID)
  set(CLANG_COMMON_FLAGS ${CLANG_COMMON_FLAGS}
	-Wno-deprecated
	-Wno-nonportable-include-path		# This will be caught be submissions tests.
    -fno-exceptions
    -fms-extensions
	-D_MSC_EXTENSIONS=1
  )
else()
  set(CLANG_COMMON_FLAGS ${CLANG_COMMON_FLAGS} 
    -msse2
    -Wunknown-attributes
    -fno-exceptions
  )
endif()

string(REPLACE ";" " " CLANG_COMMON_FLAGS     "${CLANG_COMMON_FLAGS}")
string(REPLACE ";" " " CLANG_CPP_COMMON_FLAGS "${CLANG_CPP_COMMON_FLAGS}")

set(CMAKE_CXX_FLAGS         "${CLANG_COMMON_FLAGS} ${CLANG_CPP_COMMON_FLAGS}" CACHE STRING "C++ Common Flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -D_DEBUG -DDEBUG"  CACHE STRING "C++ Debug Flags"   FORCE)
set(CMAKE_CXX_FLAGS_PROFILE "-O3 -D_PROFILE -DNDEBUG"  CACHE STRING "C++ Profile Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -D_RELEASE -DNDEBUG"  CACHE STRING "C++ Release Flags" FORCE)
set(CMAKE_CXX_STANDARD_LIBRARIES ""                    CACHE STRING "Libraries linked by defalut with all C++ applications." FORCE)

set(CMAKE_C_FLAGS         "${CLANG_COMMON_FLAGS}"      CACHE STRING "C Common Flags"  FORCE)
set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}"   CACHE STRING "C Debug Flags"   FORCE)
set(CMAKE_C_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE}" CACHE STRING "C Profile Flags" FORCE)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "C Release Flags" FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_PROFILE ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} CACHE STRING "Linker Library Profile Flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PROFILE ${CMAKE_EXE_LINKER_FLAGS_DEBUG} CACHE STRING "Linker Executable Profile Flags" FORCE)