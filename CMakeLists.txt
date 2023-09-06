include(./func.cmake)

cmake_minimum_required(VERSION 3.5)

project(tesix)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MESSAGE_CONTEXT_SHOW, TRUE)

# VARS -------------------------------------------------------------------------------
#Tree Sitter 
set(TREE_SITTER_DIR ${CMAKE_SOURCE_DIR}/lib/tree-sitter)
set(TREE_SITTER_LIB ${TREE_SITTER_DIR}/libtree-sitter.a)

# Tree Sitter CPP
set(TREE_SITTER_CPP_DIR ${CMAKE_SOURCE_DIR}/lib/tree-sitter-cpp)
# Tree Sitter Languages
set(TREE_SITTER_LANGS
    ${TREE_SITTER_CPP_DIR}
)
# Tree Sitter Lang Sources / Includes
set(TREE_SITTER_LANG_INCLUDES)
set(TREE_SITTER_LANG_SOURCES)

# TESIX Sources / Includes
set(TESIX_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(TESIX_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

message(STATUS "Finding Tree Sitter Lang Sources and Includes")
foreach(TREE_SITTER_LANG ${TREE_SITTER_LANGS})
    list(APPEND TREE_SITTER_LANG_INCLUDES ${TREE_SITTER_LANG}/src/tree-sitter)
    file(GLOB SOURCES ${TREE_SITTER_LANG}/src/*.c)
    list(APPEND TREE_SITTER_LANG_SOURCES ${SOURCES})
endforeach()

# TESIX Sources / Includes
message(STATUS "Finding TESIX Sources")
file(GLOB_RECURSE TESIX_SOURCES ${TESIX_SOURCE_DIR}/*.cpp)
message(STATUS "Finding TESIX Includes")
GETSUBDIRS(TESIX_INCLUDES ${TESIX_INCLUDE_DIR} true)

# Main Function (cuz why not)
set(MAIN ${CMAKE_SOURCE_DIR}/main.cpp)

# Project name
set(PROJ ${CMAKE_PROJECT_NAME})
# VARS -------------------------------------------------------------------------------

# Compile LibTreeSitter
message(STATUS "Building libtree-sitter")
execute_process(COMMAND make WORKING_DIRECTORY ${TREE_SITTER_DIR})

message(STATUS "Finding libncurses")
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
 
add_executable(${PROJ}
    ${MAIN}

    ${TESIX_SOURCES}

    ${TREE_SITTER_LANG_SOURCES}
)

target_include_directories(${PROJ}  PUBLIC
    ${TESIX_INCLUDES}
    
    ${TREE_SITTER_LANG_INCLUDES}
)

set_target_properties(${PROJ}  PROPERTIES OUTPUT_NAME "out")

target_link_libraries(${PROJ} 
    ${TREE_SITTER_LIB}
    ${CURSES_LIBRARIES}
)
