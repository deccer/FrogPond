include(FetchContent)

#- GLFW-----------------------------------------------------------------------------------------------------------------

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_INCLUDE_NONE ON CACHE BOOL "" FORCE)
add_subdirectory(glfw)

#- GLM -----------------------------------------------------------------------------------------------------------------

add_subdirectory(glm)
target_compile_definitions(glm INTERFACE
    GLM_FORCE_AVX2
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

#- SPDLOG --------------------------------------------------------------------------------------------------------------

set(SPDLOG_NO_EXCEPTIONS ON CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog)
target_compile_options(spdlog PRIVATE $<$<BOOL:${MSVC}>:/w>)
target_compile_definitions(spdlog PRIVATE FMT_EXCEPTIONS=0)

add_subdirectory(glad/cmake glad_cmake)
glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.6)

#- Dear ImGUI ----------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    message("Fetching imgui")
    FetchContent_Populate(imgui)

    add_library(imgui
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)

    target_include_directories(imgui PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
            ${glfw_SOURCE_DIR}/include)

    target_link_libraries(imgui PRIVATE glfw)
endif ()

#- ImPlot --------------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG        v0.16
)

FetchContent_GetProperties(implot)
if(NOT implot_POPULATED)
    FetchContent_Populate(implot)

    add_library(implot
        ${implot_SOURCE_DIR}/implot.cpp
        ${implot_SOURCE_DIR}/implot_items.cpp
        ${implot_SOURCE_DIR}/implot_demo.cpp
    )

    target_include_directories(implot PUBLIC 
        ${implot_SOURCE_DIR}
        ${imgui_SOURCE_DIR} # ImPlot must see ImGui headers
    )
endif()

set(IMPLOT_SOURCE ${implot_SOURCE_DIR} PARENT_SCOPE)