set(FETCHCONTENT_QUIET ON CACHE BOOL "" FORCE)
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
    GLM_FORCE_SILENT_WARNINGS
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
    message("Fetching implot")
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

#- fastgltf ------------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG        v0.7.1
)

message("Fetching fastgltf")
FetchContent_MakeAvailable(fastgltf)

#- MeshOptimizer -------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG        v0.19
)
message("Fetching meshoptimizer")
FetchContent_MakeAvailable(meshoptimizer)

#- Ktx -----------------------------------------------------------------------------------------------------------------

# KTX changes global variables CMAKE_ARCHIVE_OUTPUT_DIRECTORY, CMAKE_LIBRARY_OUTPUT_DIRECTORY and
# CMAKE_RUNTIME_OUTPUT_DIRECTORY, save them and restore later
set(TOADWART_TEMPORARY_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(TOADWART_TEMPORARY_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(TOADWART_TEMPORARY_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

FetchContent_Declare(
    ktx
    GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
    GIT_TAG        v4.3.2
)
set(KTX_FEATURE_TESTS OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_KTX1 ON CACHE BOOL "" FORCE)
set(KTX_FEATURE_KTX2 ON CACHE BOOL "" FORCE)
set(KTX_FEATURE_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
set(KTX_FEATURE_VK_UPLOAD OFF CACHE BOOL "" FORCE)
set(KTX_FEATURE_GL_UPLOAD OFF CACHE BOOL "" FORCE)

message("Fetching ktx")
FetchContent_MakeAvailable(ktx)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TOADWART_TEMPORARY_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TOADWART_TEMPORARY_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TOADWART_TEMPORARY_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(ktx
    PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
    -Wno-everything>
    $<$<CXX_COMPILER_ID:MSVC>:
    -w>
)

#- Stb -----------------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    stb
    GIT_REPOSITORY  https://github.com/nothings/stb.git
    GIT_TAG         master
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)
FetchContent_GetProperties(stb)
if(NOT stb)
    FetchContent_Populate(stb)
    message("Fetching stb")

    add_library(stb INTERFACE ${stb_SOURCE_DIR}/stb_image.h)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()

#- tracy ---------------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    tracy
    GIT_REPOSITORY  https://github.com/wolfpld/tracy.git
    GIT_TAG         master
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)

set(TRACY_ENABLE ON CACHE BOOL "Enable profiling")
set(TRACY_NO_SYSTEM_TRACING ON CACHE BOOL "Disable System Tracing")
set(TRACY_ONLY_IPV4 ON CACHE BOOL "IPv4 only")
set(TRACY_ONLY_LOCALHOST ON CACHE BOOL "Only Localhost")
message("Fetching tracy")
FetchContent_MakeAvailable(tracy)

#- debugbreak-----------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    debugbreak
    GIT_REPOSITORY https://github.com/scottt/debugbreak
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(debugbreak)
if(NOT debugbreak_POPULATED)
    message("Fetching debugbreak")
    FetchContent_Populate(debugbreak)

    add_library(debugbreak INTERFACE ${debugbreak_SOURCE_DIR}/debugbreak.h)
    target_include_directories(debugbreak INTERFACE ${debugbreak_SOURCE_DIR})
endif()

#- poolstl ------------------------------------------------------------------------------------------------------------

FetchContent_Declare(
    poolSTL
    GIT_REPOSITORY https://github.com/alugowski/poolSTL.git
    GIT_TAG        v0.3.5
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

message("Fetching poolSTL")
FetchContent_MakeAvailable(poolSTL)
