project(aeTests)

add_executable(aeTests "TestMain.cpp" "TestArray.h" "TestString.h")

target_link_libraries(aeTests PUBLIC aeCore doctest)
