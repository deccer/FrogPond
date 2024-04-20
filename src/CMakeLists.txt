if (TOADWART_ENABLE_LILYPAD)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_library(Lilypad STATIC
                Lilypad.cpp
        )
        target_link_libraries(Lilypad PRIVATE glfw XNVCtrl nvidia-ml)
    else()
        message("Lilypad not supported")
    endif()
else()
    message("Lilypad not supported")
endif()

add_custom_target(copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data)

add_executable(Toadwart
    Io.cpp
    Framebuffer.cpp
    DebugLabel.cpp
    Format.cpp
    Main.cpp
    ImGuiThemes.cpp
)

target_link_libraries(Toadwart 
    PRIVATE debugbreak glfw glad glm spdlog imgui implot meshoptimizer ktx fastgltf stb tbb poolSTL::poolSTL
    INTERFACE Toadwart.PreCompiledHeader)
if (TOADWART_ENABLE_PROFILER)
    target_link_libraries(Toadwart INTERFACE TracyClient)
endif()

set(TOADWART_CONFIGURATION_COMPILE_DEFINITIONS)
if (TOADWART_ENABLE_PROFILER)
    set(TOADWART_CONFIGURATION_COMPILE_DEFINITIONS ${TOADWART_CONFIGURATION_COMPILE_DEFINITIONS} "TOADWART_ENABLE_PROFILER")
endif()

if (TOADWART_ENABLE_LOGGER)
    set(TOADWART_CONFIGURATION_COMPILE_DEFINITIONS ${TOADWART_CONFIGURATION_COMPILE_DEFINITIONS} "TOADWART_ENABLE_LOGGER")
endif()

if (TOADWART_ENABLE_LILYPAD)
    set(TOADWART_CONFIGURATION_COMPILE_DEFINITIONS ${TOADWART_CONFIGURATION_COMPILE_DEFINITIONS} "TOADWART_ENABLE_LILYPAD")
    target_link_libraries(Toadwart PRIVATE Lilypad)    
endif()

if (WIN32)
    set(TOADWART_CONFIGURATION_COMPILE_DEFINITIONS ${TOADWART_CONFIGURATION_COMPILE_DEFINITIONS} "_CRT_SECURE_NO_WARNINGS")
endif()

target_compile_definitions(Toadwart INTERFACE ${TOADWART_CONFIGURATION_COMPILE_DEFINITIONS})

add_dependencies(Toadwart copy_data)