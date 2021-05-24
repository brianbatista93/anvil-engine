project (aeCore)

file(GLOB SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Containers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory/Allocators/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Misc/*.cpp"
)

file(GLOB HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Containers/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory/Allocators/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Misc/*.h"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_precompile_headers(${PROJECT_NAME} 
    PUBLIC 
        CorePCH.h 
)

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER Runtime
)