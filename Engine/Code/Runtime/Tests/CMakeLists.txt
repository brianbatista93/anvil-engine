project(aeTests)

add_executable(UnitTests "TestMain.cpp" "TestArray.h")

target_link_libraries(UnitTests aeCore doctest)
