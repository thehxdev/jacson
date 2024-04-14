cmake_minimum_required(VERSION 3.15)

project(
    jacson
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
add_compile_options(
    -Wall
    -Wextra
    -Wno-unused-result
)

if (OPTIMIZE)
    add_compile_options(-O2 -DNDEBUG)
    add_link_options(-s)
else()
    add_compile_options(-Og -ggdb)
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()


add_library(jacson
    src/jacson.c
    src/parser.c
    src/jvalue.c
    src/lexer.c
    src/str.c
    src/query.c
    src/validator.c
)
target_include_directories(jacson PRIVATE "./include")

if (TRACE)
    target_compile_definitions(jacson PUBLIC __JCSN_TRACE__)
endif()


add_executable(test
    test/test.c
)
target_include_directories(test PRIVATE "./include")
target_link_libraries(test PRIVATE jacson)
