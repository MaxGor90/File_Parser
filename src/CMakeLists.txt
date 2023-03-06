cmake_minimum_required(VERSION 3.0.0)
project(File_Parser VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_SOURCE_DIR ${WORKING_DIRECTORY}/src)
file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS RELATIVE ${CMAKE_SOURCE_DIR} "*.cpp" "*.h")

add_executable(File_Parser ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC pqxx pq ws2_32 wsock32)

target_include_directories(${PROJECT_NAME} PUBLIC "E:/Program Files/PostgreSQL/15/include"
                                                    "E:/Program Files/PostgreSQL/pqxx/include")

target_link_directories(${PROJECT_NAME} PUBLIC      "E:/Program Files/PostgreSQL/15/lib"
                                                    "E:/Program Files/PostgreSQL/pqxx/lib")



target_compile_features(File_Parser PUBLIC cxx_std_17)
target_compile_options(File_Parser PUBLIC -Wall -Wextra -Wpedantic -ggdb)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
