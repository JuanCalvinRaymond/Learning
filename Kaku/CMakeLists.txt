cmake_minimum_required(VERSION 3.10.0)
project(TextEditor VERSION 0.1.0 LANGUAGES C)

add_executable(TextEditor main.c)

execute_process(
    COMMAND TextEditor Script.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)