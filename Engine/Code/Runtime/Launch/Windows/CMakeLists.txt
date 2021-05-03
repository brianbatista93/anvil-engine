project (WindowsLauncher)

set_source_files_properties("anvil.ico" "anvil.rc" PROPERTIES LANGUAGE RC)

add_executable(${PROJECT_NAME} WIN32 "WindowsLauncher.cpp" "anvil.rc" )

get_filename_component(WINDOWS_SDK "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE CACHE)

target_link_libraries(${PROJECT_NAME} PRIVATE aeCore)