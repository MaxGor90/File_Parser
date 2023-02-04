cmake_minimum_required(VERSION 3.0.0)
project(File_Parser VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS RELATIVE ${CMAKE_SOURCE_DIR} "*.cpp" "*.h")

add_executable(File_Parser ${SOURCES})

target_compile_features(File_Parser PUBLIC cxx_std_17)
target_compile_options(File_Parser PUBLIC -Wall -Wextra -Wpedantic -ggdb)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
