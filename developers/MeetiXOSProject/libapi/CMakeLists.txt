cmake_minimum_required(VERSION 2.8.4)

project(mxapi)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(mxapi STATIC ${SOURCES})
set_target_properties(mxapi PROPERTIES COMPILE_FLAGS "-std=c++1y")
target_include_directories(mxapi PUBLIC "inc")
