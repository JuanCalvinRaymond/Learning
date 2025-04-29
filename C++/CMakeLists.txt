cmake_minimum_required(VERSION 3.10.0)

#add_subdirectory(thirdparty/libsound)

project(HelloWorld VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
file (GLOB_RECURSE MYSOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable("${CMAKE_PROJECT_NAME}" "${MYSOURCES}")

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
# target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/")

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
#target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE libsound.so)

include(CTest)
enable_testing()

