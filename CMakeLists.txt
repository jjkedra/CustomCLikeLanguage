cmake_minimum_required(VERSION 3.25)
project(lexer)
set(CMAKE_CXX_STANDARD 17 "-Wall -Wextra -Wconversion -std=c++17")
add_definitions(-Werror)

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(include)
include_directories(include/Visitors)


add_executable(main main.cpp tests/test.cpp)
add_executable(google_test tests/test.cpp)
add_library(Lexer
        include/Lexer.h
        src/Lexer.cpp
        include/LangException.h
        src/LangException.cpp
        include/Position.h
        src/Position.cpp
        include/Token.h
        src/Token.cpp
        util/utils.h
        util/utils.cpp
        include/Tree.h
        src/Tree.cpp
        include/Visitors/AstVisitor.h
        src/Visitors/AstVisitor.cpp
        include/Parser.h
        src/Parser.cpp
        include/Visitors/ParserVisitor.h
        src/Visitors/ParserVisitor.cpp
        )
target_include_directories(Lexer PUBLIC include)

# Below for Linux
#target_link_libraries(main Lexer gtest_main gtest)
#target_link_libraries(google_test Lexer gtest_main gtest)

# Below for MacOS X
 target_link_libraries(main Lexer)
 target_link_libraries(google_test Lexer)

# Link against Google Test
target_link_libraries(main ${GTEST_LIBRARIES} pthread)
target_link_libraries(google_test ${GTEST_LIBRARIES} pthread)
