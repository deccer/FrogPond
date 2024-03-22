#include <cstdint>
#include <spdlog/spdlog.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <unordered_map>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <string_view>
#include <expected>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum class EWindowStyle {
    Windowed,
    Fullscreen,
    FullscreenExclusive
};

struct SWindowSettings {
    int32_t ResolutionWidth;
    int32_t ResolutionHeight;
    float ResolutionScale;
    EWindowStyle WindowStyle;
    bool IsDebug;
};

GLFWwindow* g_window = nullptr;
ImGuiContext* g_imguiContext = nullptr;
int32_t g_framebufferWidth = 0;
int32_t g_framebufferHeight = 0;

struct SVertexPositionColor {
    glm::vec3 Position;
    glm::vec4 Color;
};

static auto ReadTextFromFile(const std::string_view filePath) -> std::string {

    std::ifstream file(filePath.data(), std::ios::ate);
    std::string result(file.tellg(), '\0');
    file.seekg(0);
    file.read(result.data(), result.size());
    return result;
}

auto SetDebugLabel(
    const uint32_t object,
    const uint32_t objectType,
    const std::string_view label) -> void {

    glObjectLabel(objectType, object, static_cast<GLsizei>(label.size()), label.data());
}

auto CreateProgram(
    const uint32_t shaderType,
    const std::string_view shaderSource,
    const std::string_view label) -> std::expected<uint32_t, std::string> {

    const auto shaderSourcePtr = shaderSource.data();
    auto program = glCreateShaderProgramv(shaderType, 1, &shaderSourcePtr);
    SetDebugLabel(program, GL_PROGRAM, label);
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

    int32_t linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        std::string errorLog;
        int32_t length = 512;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        errorLog.resize(length + 1, '\0');
        glGetProgramInfoLog(program, length, nullptr, errorLog.data());
        return std::unexpected(errorLog);
    }

    return program;
}

auto CreateProgramPipeline(
    const std::string_view label,
    const std::string_view vertexShaderFilePath,
    const std::string_view fragmentShaderFilePath) -> std::expected<uint32_t, std::string> {

    const auto vertexShaderSource = ReadTextFromFile(vertexShaderFilePath);
    const auto fragmentShaderSource = ReadTextFromFile(fragmentShaderFilePath);

    auto vertexShaderResult = CreateProgram(GL_VERTEX_SHADER, vertexShaderSource, vertexShaderFilePath);
    if (!vertexShaderResult) {
        return std::unexpected(vertexShaderResult.error());
    }

    auto fragmentShaderResult = CreateProgram(GL_FRAGMENT_SHADER, fragmentShaderSource, fragmentShaderFilePath);
    if (!fragmentShaderResult) {
        return std::unexpected(fragmentShaderResult.error());
    }

    uint32_t pipeline = 0;
    glCreateProgramPipelines(1, &pipeline);
    SetDebugLabel(pipeline, GL_PROGRAM_PIPELINE, label);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, *vertexShaderResult);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, *fragmentShaderResult);

    return pipeline;
}

auto CreateProgramPipeline(
    const std::string_view label,
    const std::string_view computeShaderFilePath) -> std::expected<uint32_t, std::string> {

    const auto computeShaderSource = ReadTextFromFile(computeShaderFilePath);
    auto computeShaderResult = CreateProgram(GL_COMPUTE_SHADER, computeShaderSource, computeShaderFilePath);
    if (!computeShaderResult) {
        return std::unexpected(computeShaderResult.error());
    }

    uint32_t pipeline = 0;
    glCreateProgramPipelines(1, &pipeline);
    SetDebugLabel(GL_PROGRAM_PIPELINE, pipeline, label);
    glUseProgramStages(pipeline, GL_COMPUTE_SHADER_BIT, *computeShaderResult);

    return pipeline;
}

auto OnKey(
    GLFWwindow* window,
    const int32_t key,
    [[maybe_unused]] int32_t scancode,
    const int32_t action,
    [[maybe_unused]] int32_t mods) -> void {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

auto OnWindowFramebufferSizeChanged(
    [[maybe_unused]] GLFWwindow* window,
    const int width,
    const int height) -> void {

    g_framebufferWidth = width;
    g_framebufferHeight = height;
}

auto OnOpenGLDebugMessage(
    uint32_t source,
    uint32_t type,
    uint32_t id,
    uint32_t severity, 
    int32_t length,
    const char* message,
    const void* userParam) -> void {

    if (type == GL_DEBUG_TYPE_ERROR) {
        spdlog::error(message);
        __asm__ volatile("int $0x03");
    }
}

auto main() -> int32_t
{
    constexpr SWindowSettings windowSettings = {
        .ResolutionWidth = 1680,
        .ResolutionHeight = 1050,
        .ResolutionScale = 1.0f,
        .WindowStyle = EWindowStyle::Windowed,
        .IsDebug = true
    };

    if (glfwInit() == GLFW_FALSE) {
        return -1;
    }

    constexpr auto isWindowWindowed = windowSettings.WindowStyle == EWindowStyle::Windowed;
    glfwWindowHint(GLFW_DECORATED, isWindowWindowed ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, isWindowWindowed ? GLFW_TRUE : GLFW_FALSE);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const auto primaryMonitor = glfwGetPrimaryMonitor();
    if (primaryMonitor == nullptr) {
        spdlog::error("Unable to get primary monitor");
        return -2;
    }

    const auto primaryMonitorVideoMode = glfwGetVideoMode(primaryMonitor);
    const auto screenWidth = primaryMonitorVideoMode->width;
    const auto screenHeight = primaryMonitorVideoMode->height;

    constexpr auto windowWidth = windowSettings.ResolutionWidth;
    constexpr auto windowHeight = windowSettings.ResolutionHeight;

    GLFWmonitor* monitor = windowSettings.WindowStyle == EWindowStyle::FullscreenExclusive
        ? primaryMonitor
        : nullptr;

    g_window = glfwCreateWindow(windowWidth, windowHeight, "FrogPond", monitor, nullptr);
    if (g_window == nullptr) {
        spdlog::error("Unable to create a window");
        return -3;
    }

    int32_t monitorLeft = 0;
    int32_t monitorTop = 0;
    glfwGetMonitorPos(primaryMonitor, &monitorLeft, &monitorTop);
    if (isWindowWindowed) {
        glfwSetWindowPos(g_window, screenWidth / 2 - windowWidth / 2 + monitorLeft, screenHeight / 2 - windowHeight / 2 + monitorTop);
    } else {
        glfwSetWindowPos(g_window, monitorLeft, monitorTop);
    }

    glfwSetKeyCallback(g_window, OnKey);
    glfwSetFramebufferSizeCallback(g_window, OnWindowFramebufferSizeChanged);

    int32_t framebufferWidth = 0;
    int32_t framebufferHeight = 0;
    glfwGetFramebufferSize(g_window, &framebufferWidth, &framebufferHeight);
    OnWindowFramebufferSizeChanged(g_window, framebufferWidth, framebufferHeight);

    glfwMakeContextCurrent(g_window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        spdlog::error("Unable to initialize glad");
        return -4;
    }

    if (windowSettings.IsDebug) {
        glDebugMessageCallback(OnOpenGLDebugMessage, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }

    auto g_imguiContext = ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(g_window, true)) {
        spdlog::error("Unable to Init ImGui's GLFW Backend");
        return -5;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 460")) {
        spdlog::error("nable to initialize OpenGL");
        return -6;
    }

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glViewport(0, 0, g_framebufferWidth, g_framebufferHeight);

    uint32_t defaultInputLayout = 0;
    glCreateVertexArrays(1, &defaultInputLayout);
    SetDebugLabel(defaultInputLayout, GL_VERTEX_ARRAY, "InputLayout");
    glBindVertexArray(defaultInputLayout);

    glVertexArrayAttribFormat(defaultInputLayout, 0, 3, GL_FLOAT, false, offsetof(SVertexPositionColor, Position));
    glVertexArrayAttribBinding(defaultInputLayout, 0, 0);
    glEnableVertexArrayAttrib(defaultInputLayout, 0);

    glVertexArrayAttribFormat(defaultInputLayout, 1, 4, GL_FLOAT, false, offsetof(SVertexPositionColor, Color));
    glVertexArrayAttribBinding(defaultInputLayout, 1, 0);
    glEnableVertexArrayAttrib(defaultInputLayout, 1);

    std::vector<SVertexPositionColor> vertices = {
        SVertexPositionColor{{+0.0f, +0.5f, +0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        SVertexPositionColor{{-0.5f, -0.5f, +0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        SVertexPositionColor{{+0.5f, -0.5f, +0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    };
    std::vector<uint32_t> indices = { 0, 1, 2};

    uint32_t vertexBuffer = 0;
    glCreateBuffers(1, &vertexBuffer);
    SetDebugLabel(vertexBuffer, GL_BUFFER, "VertexBuffer");
    glNamedBufferData(vertexBuffer, vertices.size() * sizeof(SVertexPositionColor), vertices.data(), GL_STATIC_DRAW);

    uint32_t indexBuffer = 0;
    glCreateBuffers(1, &indexBuffer);
    SetDebugLabel(indexBuffer, GL_BUFFER, "IndexBuffer");
    glNamedBufferData(indexBuffer, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    auto simpleProgramResult = CreateProgramPipeline(
        "SimplePipeline",
        "data/shaders/Simple.vs.glsl",
        "data/shaders/Simple.fs.glsl");
    if (!simpleProgramResult) {
        spdlog::error(simpleProgramResult.error());
        return -7;
    }

    auto simpleProgram = *simpleProgramResult;

    glClearColor(0.04f, 0.05f, 0.06f, 1.0f);

    while (!glfwWindowShouldClose(g_window)) {
        glfwPollEvents();
        glEnable(GL_FRAMEBUFFER_SRGB);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindProgramPipeline(simpleProgram);

        glVertexArrayVertexBuffer(defaultInputLayout, 0, vertexBuffer, 0, sizeof(SVertexPositionColor));
        glVertexArrayElementBuffer(defaultInputLayout, indexBuffer);

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        glDisable(GL_FRAMEBUFFER_SRGB);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Huhu")) {

        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(g_window);
    }

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &defaultInputLayout);
    glDeleteProgramPipelines(1, &simpleProgram);

    if (g_imguiContext != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(g_imguiContext);
    }

    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}
