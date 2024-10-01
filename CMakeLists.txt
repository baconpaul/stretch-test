cmake_minimum_required(VERSION 3.10)
project(stretch-test VERSION 1.0 LANGUAGES C CXX)

add_subdirectory(libs/libsndfile)

add_library(ssstretch INTERFACE)
target_include_directories(ssstretch INTERFACE libs/signalsmith-stretch)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE sndfile ssstretch)
