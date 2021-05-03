project(feTests)

add_executable(UnitTests "TestMain.cpp" "TestArray.h" "TestString.h" "TestQueue.h")

target_link_libraries(UnitTests feCore doctest)
