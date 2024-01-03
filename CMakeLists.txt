cmake_minimum_required(VERSION 3.28)

project(Jet)

# Output all binary files to the bin/{Config} folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

add_subdirectory(./Projects)
add_subdirectory(./thirdparty)