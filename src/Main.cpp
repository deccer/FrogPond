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
#include <glm/gtc/color_space.hpp>

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
glm::ivec2 g_framebufferSize = glm::ivec2{0, 0};
glm::ivec2 g_sceneViewerSize = glm::ivec2{0, 0};
bool g_framebufferResized = true;
bool g_sceneViewerResized = false;
bool g_isEditor = false;

uint32_t g_defaultInputLayout = 0;
uint32_t g_vertexPositionColorInputLayout = 0;
uint32_t g_vertexPositionUvInputLayout = 0;
uint32_t g_fullscreenTrianglePipeline = 0;
uint32_t g_samplerNearestNearestClampToEdge = 0;

struct SVertexPositionUv {
    glm::vec3 Position;
    glm::vec2 Uv;
};

struct SVertexPositionColor {
    glm::vec3 Position;
    glm::vec4 Color;
};

struct SCameraInformation {
    glm::mat4x4 ProjectionMatrix;
    glm::mat4x4 ViewMatrix;
};

struct SObject {
    glm::mat4x4 WorldMatrix;
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
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        g_isEditor = !g_isEditor;
        g_framebufferResized = !g_isEditor;
        g_sceneViewerResized = g_isEditor;
    }
}

auto OnWindowFramebufferSizeChanged(
    [[maybe_unused]] GLFWwindow* window,
    const int width,
    const int height) -> void {

    g_framebufferSize = glm::ivec2{width, height};
    g_framebufferResized = true;
}

auto OnOpenGLDebugMessage(
    [[maybe_unused]] uint32_t source,
    uint32_t type,
    [[maybe_unused]] uint32_t id,
    [[maybe_unused]] uint32_t severity, 
    [[maybe_unused]] int32_t length,
    const char* message,
    [[maybe_unused]] const void* userParam) -> void {

    if (type == GL_DEBUG_TYPE_ERROR) {
        spdlog::error(message);
        __asm__ volatile("int $0x03"); // portable enough for lunix and binbows
    }
}

auto static CreateFullscreenTriangleProgramPipeline() -> uint32_t
{
    auto fullscreenTrianglePipelineResult = CreateProgramPipeline("FST", "data/shaders/FST.vs.glsl", "data/shaders/FST.fs.glsl");
    if (!fullscreenTrianglePipelineResult) {
        auto errorMessage = fullscreenTrianglePipelineResult.error();
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, errorMessage.size(), errorMessage.data());
    }
    return *fullscreenTrianglePipelineResult;
}

auto static CreateTextureSampler() -> uint32_t {
    uint32_t sampler = 0;
    glCreateSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, 0.0f);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_LOD, -1000);
    glSamplerParameteri(sampler, GL_TEXTURE_MAX_LOD, 1000);
    return sampler;
}

auto DrawFullscreenTriangle(uint32_t texture) -> void {
    
    glBindProgramPipeline(g_fullscreenTrianglePipeline);
    glBindTextureUnit(0, texture);
    glBindSampler(0, g_samplerNearestNearestClampToEdge);
    glBindVertexArray(g_defaultInputLayout);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

auto main() -> int32_t
{
    SWindowSettings windowSettings = {
        .ResolutionWidth = 1680,
        .ResolutionHeight = 720,
        .ResolutionScale = 1.0f,
        .WindowStyle = EWindowStyle::Windowed,
        .IsDebug = true
    };

    if (glfwInit() == GLFW_FALSE) {
        return -1;
    }

    const auto isWindowWindowed = windowSettings.WindowStyle == EWindowStyle::Windowed;
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

    const auto windowWidth = windowSettings.ResolutionWidth;
    const auto windowHeight = windowSettings.ResolutionHeight;

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(g_window, true)) {
        spdlog::error("Unable to Init ImGui's GLFW Backend");
        return -5;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 460")) {
        spdlog::error("nable to initialize OpenGL");
        return -6;
    }

    glfwSwapInterval(1);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glViewport(0, 0, g_framebufferSize.x, g_framebufferSize.y);

    glCreateVertexArrays(1, &g_vertexPositionColorInputLayout);
    SetDebugLabel(g_vertexPositionColorInputLayout, GL_VERTEX_ARRAY, "InputLayout_VertexPositionColor");

    glVertexArrayAttribFormat(g_vertexPositionColorInputLayout, 0, 3, GL_FLOAT, false, offsetof(SVertexPositionColor, Position));
    glVertexArrayAttribBinding(g_vertexPositionColorInputLayout, 0, 0);
    glEnableVertexArrayAttrib(g_vertexPositionColorInputLayout, 0);

    glVertexArrayAttribFormat(g_vertexPositionColorInputLayout, 1, 4, GL_FLOAT, false, offsetof(SVertexPositionColor, Color));
    glVertexArrayAttribBinding(g_vertexPositionColorInputLayout, 1, 0);
    glEnableVertexArrayAttrib(g_vertexPositionColorInputLayout, 1);

    std::vector<SVertexPositionColor> vertices = {
        SVertexPositionColor{{+0.0f, +0.5f, +0.0f}, {0.6f, 0.1f, 0.8f, 1.0f}},
        SVertexPositionColor{{-0.5f, -0.5f, +0.0f}, {0.1f, 0.8f, 0.6f, 1.0f}},
        SVertexPositionColor{{+0.5f, -0.5f, +0.0f}, {0.8f, 0.6f, 0.1f, 1.0f}},
    };
    std::vector<uint32_t> indices = {0, 1, 2};

    uint32_t vertexBuffer = 0;
    glCreateBuffers(1, &vertexBuffer);
    SetDebugLabel(vertexBuffer, GL_BUFFER, "VertexBuffer");
    glNamedBufferData(vertexBuffer, vertices.size() * sizeof(SVertexPositionColor), vertices.data(), GL_STATIC_DRAW);

    uint32_t indexBuffer = 0;
    glCreateBuffers(1, &indexBuffer);
    SetDebugLabel(indexBuffer, GL_BUFFER, "IndexBuffer");
    glNamedBufferData(indexBuffer, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    auto simpleProgramPipelineResult = CreateProgramPipeline(
        "SimplePipeline",
        "data/shaders/Simple.vs.glsl",
        "data/shaders/Simple.fs.glsl");
    if (!simpleProgramPipelineResult) {
        spdlog::error(simpleProgramPipelineResult.error());
        return -7;
    }

    auto simpleProgramPipeline = *simpleProgramPipelineResult;

    auto cameraPosition = glm::vec3{0, 0, 10};
    auto cameraDirection = glm::vec3{0, 0, -1};
    auto cameraUp = glm::vec3{0, 1, 0};
    SCameraInformation cameraInformation = {
        .ProjectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(60.0f), (float)g_framebufferSize.x, (float)g_framebufferSize.x, 0.1f, 1024.0f),
        .ViewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp)
    };

    uint32_t cameraInformationBuffer = 0;
    glCreateBuffers(1, &cameraInformationBuffer);
    SetDebugLabel(cameraInformationBuffer, GL_BUFFER, "CameraInformation");
    glNamedBufferData(cameraInformationBuffer, sizeof(SCameraInformation), &cameraInformation, GL_DYNAMIC_DRAW);

    auto CreateWorldMatrix = [](float x, float y, float z) {
        return glm::translate(glm::mat4x4(1.0f), glm::vec3(x, y, z));
    };

    std::vector<SObject> objects = {
        {.WorldMatrix = CreateWorldMatrix(-2.0f, -2.0f, 0.0f)},
        {.WorldMatrix = CreateWorldMatrix(+2.0f, -2.0f, 0.0f)},
        {.WorldMatrix = CreateWorldMatrix(-2.0f, +2.0f, 0.0f)},
        {.WorldMatrix = CreateWorldMatrix(+2.0f, +2.0f, 0.0f)},
    };

    uint32_t objectBuffer = 0;
    glCreateBuffers(1, &objectBuffer);
    SetDebugLabel(objectBuffer, GL_BUFFER, "Objects");
    glNamedBufferData(objectBuffer, sizeof(SObject) * objects.size(), objects.data(), GL_DYNAMIC_DRAW);

    uint32_t mainTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &mainTexture);
    SetDebugLabel(mainTexture, GL_TEXTURE, std::format("MainTexture_{}x{}", g_framebufferSize.x, g_framebufferSize.y));
    glTextureStorage2D(mainTexture, 1, GL_RGBA8, g_framebufferSize.x, g_framebufferSize.y);

    uint32_t mainFramebuffer = 0;
    glCreateFramebuffers(1, &mainFramebuffer);
    SetDebugLabel(mainTexture, GL_FRAMEBUFFER, "MainFramebuffer");    
    glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainTexture, 0);

    auto mainFramebufferStatus = glCheckNamedFramebufferStatus(mainFramebuffer, GL_FRAMEBUFFER);
    if (mainFramebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        auto errorMessage = std::string("Framebuffer incomplete");
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, errorMessage.size(), errorMessage.data());
        return -10;
    }
    glNamedFramebufferDrawBuffer(mainFramebuffer, GL_COLOR_ATTACHMENT0);

    g_fullscreenTrianglePipeline = CreateFullscreenTriangleProgramPipeline();
    g_samplerNearestNearestClampToEdge = CreateTextureSampler();
    glCreateVertexArrays(1, &g_defaultInputLayout);
    SetDebugLabel(g_defaultInputLayout, GL_VERTEX_ARRAY, "InputLayout_Empty");

    auto lastProgramPipeline = 0;
    auto lastFramebuffer = 0;
    auto lastInputLayout = 0;

    auto isSrgbDisabled = false;
    auto isCullfaceDisabled = false;

    g_sceneViewerSize = g_framebufferSize;

    auto previousTimeInSeconds = glfwGetTime();
    while (!glfwWindowShouldClose(g_window)) {

        auto currentTimeInSeconds = glfwGetTime();
        auto deltaTimeInSeconds = currentTimeInSeconds - previousTimeInSeconds;
        previousTimeInSeconds = currentTimeInSeconds;

        auto framebufferWasResized = false;
        if (g_isEditor) {
            auto sceneViewerSizeScaled = glm::vec2(g_sceneViewerSize) * windowSettings.ResolutionScale;
            glViewport(0, 0, sceneViewerSizeScaled.x, sceneViewerSizeScaled.y);
            if (g_sceneViewerResized) {

                glDeleteTextures(1, &mainTexture);
                glCreateTextures(GL_TEXTURE_2D, 1, &mainTexture);
                SetDebugLabel(mainTexture, GL_TEXTURE, std::format("MainTexture_{}x{}", sceneViewerSizeScaled.x, sceneViewerSizeScaled.y));
                glTextureStorage2D(mainTexture, 1, GL_SRGB8, sceneViewerSizeScaled.x, sceneViewerSizeScaled.y);            
                glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainTexture, 0);

                g_sceneViewerResized = false;
                framebufferWasResized = true;                
            }
        } else {
            auto framebufferSizeScaled = glm::vec2(g_framebufferSize) * windowSettings.ResolutionScale;
            glViewport(0, 0, framebufferSizeScaled.x, framebufferSizeScaled.y);
            if (g_framebufferResized) {
                glDeleteTextures(1, &mainTexture);
                glCreateTextures(GL_TEXTURE_2D, 1, &mainTexture);
                SetDebugLabel(mainTexture, GL_TEXTURE, std::format("MainTexture_{}x{}", framebufferSizeScaled.x, framebufferSizeScaled.y));
                glTextureStorage2D(mainTexture, 1, GL_SRGB8, framebufferSizeScaled.x, framebufferSizeScaled.y);            
                glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainTexture, 0);

                g_framebufferResized = false;
                framebufferWasResized = true;
            }
        }

        if (isSrgbDisabled) {
            glEnable(GL_FRAMEBUFFER_SRGB);
            isSrgbDisabled = false;
        }

        if (g_isEditor) {
            glClearNamedFramebufferfv(mainFramebuffer, GL_COLOR, 0, glm::value_ptr(glm::convertLinearToSRGB(glm::vec4{0.6f, 0.1f, 0.8f, 1.0f})));
        } else {
            glClearNamedFramebufferfv(mainFramebuffer, GL_COLOR, 0, glm::value_ptr(glm::convertSRGBToLinear(glm::vec4{0.6f, 0.1f, 0.8f, 1.0f})));
        }

        glBindProgramPipeline(simpleProgramPipeline);
        glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
        glBindVertexArray(g_vertexPositionColorInputLayout);
        glVertexArrayVertexBuffer(g_vertexPositionColorInputLayout, 0, vertexBuffer, 0, sizeof(SVertexPositionColor));
        glVertexArrayElementBuffer(g_vertexPositionColorInputLayout, indexBuffer);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraInformationBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, objectBuffer);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr, objects.size());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
       
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (g_isEditor) {
            ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        }

        auto isOpen = true;
        ImGui::SetNextWindowPos({32, 32});
        ImGui::SetNextWindowSize({192, 128});
        if (ImGui::Begin("Huhu", &isOpen, ImGuiWindowFlags_::ImGuiWindowFlags_Modal | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration)) {

            ImGui::TextColored(ImVec4{0.6f, 0.1f, 0.8f, 1.0f}, "Press F1 to toggle editor");
            ImGui::Separator();

            auto framesPerSecond = 1.0f / deltaTimeInSeconds;
            ImGui::Text("rps: %.2f rad/s", glm::two_pi<float>() * framesPerSecond);
            ImGui::Text("dps: %.2f °/s", glm::degrees(glm::two_pi<float>() * framesPerSecond));
            ImGui::Text("fps: %.2f", framesPerSecond);
            ImGui::Text("rpms: %.2f", framesPerSecond * 60.0f);
            ImGui::Text("ft: %.2f ms", deltaTimeInSeconds * 1000.0f);
        }
        ImGui::End();

        if (ImGui::Begin("Debug")) {
            auto resolutionScale = windowSettings.ResolutionScale;
            if (ImGui::SliderFloat("Resolution Scale", &resolutionScale, 0.1f, 3.0f))
            {
                windowSettings.ResolutionScale = resolutionScale;
                if (g_isEditor) {
                    g_sceneViewerResized = true;
                } else {
                    g_framebufferResized = true;
                }
            }
        }
        ImGui::End();

        if (g_isEditor) {            

            glDisable(GL_FRAMEBUFFER_SRGB);
            isSrgbDisabled = true;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            if (ImGui::Begin("Scene")) {
                auto availableSceneWindowSize = ImGui::GetContentRegionAvail();
                if (availableSceneWindowSize.x != g_sceneViewerSize.x || availableSceneWindowSize.y != g_sceneViewerSize.y) {
                    g_sceneViewerSize = glm::ivec2(availableSceneWindowSize.x, availableSceneWindowSize.y);
                    g_sceneViewerResized = true;
                }
                ImGui::Image(reinterpret_cast<ImTextureID>(mainTexture), availableSceneWindowSize, ImVec2(0, 1), ImVec2(1, 0));
            }
            ImGui::PopStyleVar();
            ImGui::End();
        } else {
            DrawFullscreenTriangle(mainTexture);
/*
            glBlitNamedFramebuffer(mainFramebuffer, 0,
                                   0, 0, g_framebufferSize.x, g_framebufferSize.y,
                                   0, 0, g_framebufferSize.x, g_framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);            
*/                                   
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(g_window);
        glfwPollEvents();        
    }

    glDeleteSamplers(1, &g_samplerNearestNearestClampToEdge);
    glDeleteTextures(1, &mainTexture);
    glDeleteFramebuffers(1, &mainFramebuffer);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &objectBuffer);

    glDeleteVertexArrays(1, &g_vertexPositionColorInputLayout);
    glDeleteVertexArrays(1, &g_defaultInputLayout);

    glDeleteProgramPipelines(1, &simpleProgramPipeline);
    glDeleteProgramPipelines(1, &g_fullscreenTrianglePipeline);

    if (g_imguiContext != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(g_imguiContext);
    }

    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}
