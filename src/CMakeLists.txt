add_custom_target(copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data)

add_executable(FrogPond
        Main.cpp)

target_link_libraries(FrogPond PRIVATE glfw glad glm spdlog imgui implot meshoptimizer ktx fastgltf)
add_dependencies(FrogPond copy_data)