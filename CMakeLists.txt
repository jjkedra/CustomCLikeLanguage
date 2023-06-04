cmake_minimum_required(VERSION 3.25)
project(lexer)
set(CMAKE_CXX_STANDARD 17 "-Wall -Wextra -Wconversion -std=c++23 -g -00 -fsanitize=memory")
add_definitions(-Werror)

# Find Google Test
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

include_directories(include)
include_directories(include/Visitors)


add_executable(main main.cpp tests/test.cpp)
add_executable(test tests/test.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         release-1.11.0
)
FetchContent_MakeAvailable(googletest)



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
        src/Visitors/InterpreterVisitor.cpp include/Visitors/InterpreterVisitor.h src/SymbolTable.cpp include/SymbolTable.h src/SymbolManager.cpp include/SymbolManager.h src/Visitors/SemanticAnalyzerVisitor.cpp include/Visitors/SemanticAnalyzerVisitor.h)
target_include_directories(Lexer PUBLIC include)
include(GoogleTest)

# Below for Linux
#target_link_libraries(main Lexer gtest_main gtest)
#target_link_libraries(google_test Lexer gtest_main gtest)

# Below for MacOS X
target_link_libraries(main gtest_main Lexer)
target_link_libraries(test gtest_main Lexer)


# Link against Google Test
target_link_libraries(main ${GTEST_LIBRARIES} pthread)
target_link_libraries(test ${GTEST_LIBRARIES} pthread)
