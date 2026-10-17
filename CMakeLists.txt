cmake_minimum_required(VERSION 3.14)
project(decode)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Wextra -Werror -std=c++20")

set(SOURCES biginteger.cpp)

add_executable(decode decode.cpp ${SOURCES})
add_executable(biginttest tests.cc biginteger.cpp)
add_executable(benchmark benchmark.cc biginteger.cpp)
add_executable(benchmark-fast benchmark.cc biginteger.cpp)

target_compile_options(decode PRIVATE -gdwarf-4)
target_compile_options(biginttest PRIVATE -Wno-self-assign-overloaded -gdwarf-4)
target_compile_options(benchmark-fast PRIVATE -O2)

target_link_libraries(biginttest gtest gtest_main pthread)

add_custom_target(test ALL
    COMMENT "Testing biginteger..."
    DEPENDS biginttest
    COMMAND timeout 60 ./biginttest
   
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
