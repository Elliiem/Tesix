cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tesix)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(${CMAKE_PROJECT_NAME}
    ./main.cpp
    ./lib/tree-sitter-cpp/src/parser.c
    ./lib/tree-sitter-cpp/src/scanner.c
    ./lib/tree-sitter-cpp/src/tree_sitter/parser.h
    ./lib/tree-sitter-cpp/src/tree_sitter/runtime.h
    ./src/TESIX_File.cpp
    ./src/datatypes/TESIX_Selection.cpp
    ./src/TESIX_FileWindow.cpp
    ./src/TESIX_Window.cpp
    ./src/TESIX_Editor.cpp
    ./src/TESIX_Input.cpp
    ./src/TESIX_Zerteiler.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME}  PUBLIC
    ./include
    ./include/datatypes
)

set_target_properties(${CMAKE_PROJECT_NAME}  PROPERTIES OUTPUT_NAME "out")

add_library(custom-tree-sitter SHARED IMPORTED)

set_target_properties(custom-tree-sitter PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/lib/tree-sitter/libtree-sitter.so
)

target_link_libraries(${CMAKE_PROJECT_NAME} 
    custom-tree-sitter
    ${CURSES_LIBRARIES}
)
