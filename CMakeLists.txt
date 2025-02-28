cmake_minimum_required(VERSION 3.15)

project(
    jacson
    VERSION 0.2.1
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

set(JACSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(is_gcc_like "$<COMPILE_LANG_AND_ID:C,GNU,Clang>")
set(is_msvc "$<COMPILE_LANG_AND_ID:C,MSVC>")

add_library(
    jacson
    STATIC
    src/jacson.c
    src/parser.c
    src/jvalue.c
    src/lexer.c
    src/str.c
    src/query.c
    src/validator.c
)

target_compile_options(
    jacson
    PRIVATE
    "$<${is_msvc}:$<BUILD_INTERFACE:-W3>>"
    "$<${is_gcc_like}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Werror=pointer-arith;-Wno-unused-parameter>>"

    "$<$<CONFIG:Debug>:$<${is_msvc}:$<BUILD_INTERFACE:-Od>>>"
    "$<$<CONFIG:Debug>:$<${is_gcc_like}:$<BUILD_INTERFACE:-Og;-ggdb>>>"

    "$<$<CONFIG:Release>:$<BUILD_INTERFACE:-O2>>"
)

target_compile_features(jacson PUBLIC c_std_99)

target_include_directories(jacson PUBLIC "${JACSON_INCLUDE_DIR}")

if (TRACE)
    target_compile_definitions(jacson PRIVATE __JCSN_TRACE__)
endif()


add_executable(
    test
    test/test.c
)
# target_include_directories(test PRIVATE "./include")
target_link_libraries(test PRIVATE jacson)
