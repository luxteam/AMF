cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

project(open_amf_test1)
set(AMF_NAME amf)

# Local header files here ONLY
SET(HEADERS

   )

# Local source files here
SET(SOURCES
    ../Qt/AMF_test/main_metal.cpp
    )

#include_directories(..)
add_subdirectory(../cmake ${AMF_NAME})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${AMF_NAME})