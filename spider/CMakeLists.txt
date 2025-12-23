cmake_minimum_required(VERSION 3.10)
project(spider)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/inc)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

find_package(CURL REQUIRED)

add_executable(spider ${SOURCES})

target_link_libraries(spider CURL::libcurl)
