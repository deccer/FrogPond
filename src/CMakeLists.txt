if (USE_LILLYPAD)
        if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
                add_library(Lillypad
                        Lillypad.cpp
                )
                target_link_libraries(Lillypad PRIVATE glfw XNVCtrl nvidia-ml)
        else()
                message("Lillypad not supported")
        endif()
else()
        message("Lillypad not supported")
endif()

add_custom_target(copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data)

add_executable(FrogPond
        Main.cpp
        ImGuiThemes.cpp
)

target_link_libraries(FrogPond PRIVATE debugbreak glfw glad glm spdlog imgui implot meshoptimizer ktx fastgltf stb TracyClient tbb poolSTL::poolSTL)

if (USE_LILLYPAD)
        target_compile_definitions(FrogPond PUBLIC USE_LILLYPAD=1)
        target_link_libraries(FrogPond PRIVATE Lillypad)
endif()

add_dependencies(FrogPond copy_data)