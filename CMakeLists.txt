cmake_minimum_required(VERSION 3.17)
project(cpp_object_version_controller)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*" "tests/*")

add_executable(cpp_object_version_controller ${SOURCES})