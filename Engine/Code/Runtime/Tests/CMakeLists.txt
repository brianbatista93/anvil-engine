project(aeTests)

add_executable(aeTests "TestMain.cpp" "TestArray.h" "TestString.h" "TestSet.h" "TestArrayBool.h")

target_link_libraries(aeTests PUBLIC aeCore doctest)
