cmake_minimum_required(VERSION 3.15)
project(cpp_basic)

set(CMAKE_CXX_STANDARD 17)

# import googletest
include(cmake/gtest/CMakeLists.txt)

add_executable(
        cpp_basic
        new.cpp
        auto.cpp
        member.cpp value_pointer_ref.cpp)

target_link_libraries(cpp_basic gtest gtest_main)

add_test(
        NAME CppBasic
        COMMAND cpp_basic
)
