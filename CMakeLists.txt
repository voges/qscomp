cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

# Set up project-specific variables
set(PROJECT_NAME "QScomp")
project(${PROJECT_NAME})
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(PROJECT_BUILD_DIR ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${PROJECT_ROOT_DIR}/src/QScomp)
set(PROJECT_INCLUDE_DIR ${PROJECT_ROOT_DIR}/src/QScomp)

# Get all source and header files (C and C++)
file(GLOB_RECURSE C_SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE CC_SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB_RECURSE CPP_SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE H_HEADER_FILES ${PROJECT_INCLUDE_DIR}/*.h)
file(GLOB_RECURSE HH_HEADER_FILES ${PROJECT_INCLUDE_DIR}/*.hh)
file(GLOB_RECURSE HPP_HEADER_FILES ${PROJECT_INCLUDE_DIR}/*.hpp)
set(PROJECT_SOURCE_FILES ${C_SOURCE_FILES} ${CC_SOURCE_FILES} ${CPP_SOURCE_FILES})
set(PROJECT_HEADER_FILES ${H_HEADER_FILES} ${HH_HEADER_FILES} ${HPP_HEADER_FILES})

# Group the source and header files
source_group("Source files" FILES ${PROJECT_SOURCE_FILES})
source_group("Header files" FILES ${PROJECT_HEADER_FILES})

# Set compiler flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Detected GNU Compiler Collection")

    # C compiler flags
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -02")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wfloat-equal")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wwrite-strings")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Waggregate-return")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-qual")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-enum")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunreachable-code")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wundef")

    # C++ compiler flags
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "Detected (Apple)Clang")

    # C compiler flags
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -02")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")

    # C++ compiler flags
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Detected Microsoft Visual Studio Compiler")
else ()
    message(FATAL_ERROR "Compiler not supported: ${CMAKE_CXX_COMPILER_ID}")
endif ()

# Includes, targets, and dependencies
include_directories(${PROJECT_BUILD_DIR})
include_directories(${PROJECT_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES})

