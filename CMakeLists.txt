cmake_minimum_required(VERSION 3.27)

macro(set_option option help value)
    option(${option} ${help} ${value})
    if(${option})
        message(STATUS "${option}: ON")
    else()
        message(STATUS "${option}: OFF")
    endif()
endmacro()

project(Toadwart)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "")

set(TOADWART_ENABLE_LOGGER OFF CACHE BOOL "Enable logging")
set(TOADWART_ENABLE_PROFILER ON CACHE BOOL "Enable CPU profiling")
set(TOADWART_ENABLE_LILYPAD ON CACHE BOOL "Fetch GPU Temperature and clock speeds on nvidia on linux")

include(cmake/PreCompiledHeaders.cmake)

add_subdirectory(libs)
add_subdirectory(src)
