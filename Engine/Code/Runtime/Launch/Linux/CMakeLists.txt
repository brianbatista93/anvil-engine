project (LinuxLauncher)

add_executable(${PROJECT_NAME} WIN32 "LinuxLauncher.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE aeCore)