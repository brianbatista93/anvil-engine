project (aeCore)

file(GLOB SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
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