cmake_minimum_required(VERSION 3.27)

macro(set_option option help value)
    option(${option} ${help} ${value})
    if(${option})
        message(STATUS "${option}: ON")
    else()
        message(STATUS "${option}: OFF")
    endif()
endmacro()

set_option(USE_LILLYPAD "Fetch GPU Temperature and clock speeds on nvidia on linux" OFF)

project(Toadwart)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "")

include(cmake/PreCompiledHeaders.cmake)

add_subdirectory(libs)
add_subdirectory(src)
