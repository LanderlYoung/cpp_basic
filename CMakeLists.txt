cmake_minimum_required(VERSION 3.15)
project(cpp_basic)

set(CMAKE_CXX_STANDARD 17)

# import googletest
include(cmake/gtest/CMakeLists.txt)

add_executable(
        cpp_basic
        new.cpp
        auto.cpp
        member.cpp
        value_pointer_ref.cpp
        template_enable_if.cpp
        init_list.cpp
        virtual.cpp
        exception.cpp
        rvalue.cpp
        thread_local.cpp
        v18.cpp
        v22.cpp
        trait.cpp
        boost.cpp)

FIND_PACKAGE(Boost 1.40)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

target_link_libraries(cpp_basic gtest gtest_main ${Boost_LIBRARIES})

add_test(
        NAME CppBasic
        COMMAND cpp_basic
)
