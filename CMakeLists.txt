cmake_minimum_required(VERSION 3.30)

project(DuifeneSign)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



add_subdirectory(src)

if(EXISTS ${CMAKE_SOURCE_DIR}/samples)
    add_subdirectory(samples)
endif()
