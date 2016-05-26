cmake_minimum_required(VERSION 2.8)

project(examples)

find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})
include_directories(".")

set (SOURCES_EXAMPLES 
    00-showcase.cpp
    01-basic.cpp
    02-basic.cpp
    03-users.cpp
    04-attachments.cpp
)

set (TARGETS_EXAMPLES
    00-showcase
    01-basic
    02-basic
    03-users
    04-attachments
)

foreach( name ${TARGETS_EXAMPLES} )
    add_executable(${name} ${name}.cpp)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(${name} ${CURL_LIBRARIES})
endforeach()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -pedantic")

if ("${CMAKE_MAJOR_VERSION}${CMAKE_MINOR_VERSION}" LESS 31)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_definitions(-DSLACKING_VERBOSE_OUTPUT=1)
