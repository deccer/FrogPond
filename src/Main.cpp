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
#include <iostream>
#include <sstream>
#include <fstream>
#include <string_view>
#include <expected>
#include <memory>
#include <utility>
#include <span>
#include <tuple>
#include <stack>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

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

struct SVertexPositionUv {
    glm::vec3 Position;
    glm::vec2 Uv;
};

struct SVertexPositionNormalUv {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Uv;
};

struct SVertexPositionColor {
    glm::vec3 Position;
    glm::vec4 Color;
};

struct SGlobalUniforms {
    glm::mat4 OldProjectionMatrix;
    glm::mat4 OldViewMatrix;
    glm::mat4 OldViewProjectionMatrix;
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ViewProjectionMatrix;
    glm::vec4 CameraPosition;
    glm::vec4 FrustumPlanes[6];
    glm::vec4 Viewport;
};

struct SObject {
    glm::mat4x4 WorldMatrix;
};

struct SMesh {
    size_t VertexCount;
    size_t VertexOffset;
    size_t IndexCount;
    size_t IndexOffset;
};

struct SDrawElementCommand {
    uint32_t IndexCount;
    uint32_t InstanceCount;
    uint32_t FirstIndex;
    int32_t BaseVertex;
    uint32_t BaseInstance;
};

struct SCamera {

    glm::vec3 Position = {0.0f, 0.0f, 5.0f};
    float Pitch = {};
    float Yaw = {glm::radians(-90.0f)};

    auto GetForwardDirection() -> const glm::vec3
    {
        return glm::vec3{cos(Pitch) * cos(Yaw), sin(Pitch), cos(Pitch) * sin(Yaw)};
    }

    auto GetViewMatrix() -> const glm::mat4
    {
        return glm::lookAt(Position, Position + GetForwardDirection(), glm::vec3(0, 1, 0));
    }
};

constexpr ImVec2 g_imvec2UnitX = ImVec2(1, 0);
constexpr ImVec2 g_imvec2UnitY = ImVec2(0, 1);

GLFWwindow* g_window = nullptr;
ImGuiContext* g_imguiContext = nullptr;
ImPlotContext* g_implotContext = nullptr;
glm::ivec2 g_framebufferSize = glm::ivec2{0, 0};
glm::ivec2 g_sceneViewerSize = glm::ivec2{0, 0};
bool g_framebufferResized = true;
bool g_sceneViewerResized = false;
bool g_isEditor = false;

uint32_t g_defaultInputLayout = 0;
uint32_t g_fullscreenTrianglePipeline = 0;
uint32_t g_samplerNearestNearestClampToEdge = 0;

SCamera g_mainCamera = {};
glm::dvec2 g_cursorPosition = {};
glm::dvec2 g_cursorFrameOffset = {};
float g_cursorSensitivity = 0.0025f;
float g_cameraSpeed = 4.0f;

bool g_cursorIsActive = true;
bool g_cursorJustEntered = false;

uint32_t g_lastVertexOffset = 0;
uint32_t g_lastIndexOffset = 0;

auto inline ReadTextFromFile(const std::filesystem::path& filePath) -> std::string {

    std::ifstream fileStream{filePath};
    return {std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>()};
}

auto inline ReadBinaryFromFile(const std::filesystem::path& filePath) -> std::pair<std::unique_ptr<std::byte[]>, std::size_t>
{
    std::size_t fileSize = std::filesystem::file_size(filePath);
    auto memory = std::make_unique<std::byte[]>(fileSize);
    std::ifstream file{filePath, std::ifstream::binary};
    std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), reinterpret_cast<char*>(memory.get()));
    return {std::move(memory), fileSize};
}

auto inline SetDebugLabel(
    const uint32_t object,
    const uint32_t objectType,
    const std::string_view label) -> void {

    glObjectLabel(objectType, object, static_cast<GLsizei>(label.size()), label.data());
}

auto inline PushDebugGroup(const std::string_view label) -> void {
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, label.size(), label.data());
}

auto inline PopDebugGroup() -> void {
    glPopDebugGroup();
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

auto OnCursorEntered(
    [[maybe_unused]] GLFWwindow* window,
    int entered) -> void {

    if (entered) {
        g_cursorJustEntered = true;
    }
}

auto OnCursorPosition(
    [[maybe_unused]] GLFWwindow* window,
    double cursorPositionX,
    double cursorPositionY) -> void {

    if (g_cursorJustEntered)
    {
        g_cursorPosition = {cursorPositionX, cursorPositionY};
        g_cursorJustEntered = false;
    }        

    g_cursorFrameOffset += glm::dvec2{
        cursorPositionX - g_cursorPosition.x, 
        g_cursorPosition.y - cursorPositionY};
    g_cursorPosition = {cursorPositionX, cursorPositionY};
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

auto CreateFullscreenTriangleProgramPipeline() -> uint32_t
{
    auto fullscreenTrianglePipelineResult = CreateProgramPipeline("FST", "data/shaders/FST.vs.glsl", "data/shaders/FST.fs.glsl");
    if (!fullscreenTrianglePipelineResult) {
        auto errorMessage = fullscreenTrianglePipelineResult.error();
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, errorMessage.size(), errorMessage.data());
    }
    return *fullscreenTrianglePipelineResult;
}

auto CreateTextureSampler() -> uint32_t {
    uint32_t sampler = 0;
    glCreateSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

auto HandleCamera(float deltaTimeInSeconds) -> void {

    g_cursorIsActive = glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE;
    glfwSetInputMode(g_window, GLFW_CURSOR, g_cursorIsActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    if (!g_cursorIsActive)
    {
        glfwSetCursorPos(g_window, 0, 0);
        g_cursorPosition.x = 0;
        g_cursorPosition.y = 0;
    }

    if (!g_cursorIsActive) {

        const glm::vec3 forward = g_mainCamera.GetForwardDirection();
        const glm::vec3 right = glm::normalize(glm::cross(forward, {0, 1, 0}));

        float tempCameraSpeed = g_cameraSpeed;
        if (glfwGetKey(g_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            tempCameraSpeed *= 4.0f;
        }
        if (glfwGetKey(g_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            tempCameraSpeed *= 0.25f;
        }
        if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS) {
            g_mainCamera.Position += forward * deltaTimeInSeconds * tempCameraSpeed;
        }
        if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS) {
            g_mainCamera.Position -= forward * deltaTimeInSeconds * tempCameraSpeed;
        }
        if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS) {
            g_mainCamera.Position += right * deltaTimeInSeconds * tempCameraSpeed;
        }
        if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS) {
            g_mainCamera.Position -= right * deltaTimeInSeconds * tempCameraSpeed;
        }
        if (glfwGetKey(g_window, GLFW_KEY_Q) == GLFW_PRESS) {
            g_mainCamera.Position.y -= deltaTimeInSeconds * tempCameraSpeed;
        }
        if (glfwGetKey(g_window, GLFW_KEY_E) == GLFW_PRESS) {
            g_mainCamera.Position.y += deltaTimeInSeconds * tempCameraSpeed;
        }

        g_mainCamera.Yaw += static_cast<float>(g_cursorFrameOffset.x * g_cursorSensitivity);
        g_mainCamera.Pitch += static_cast<float>(g_cursorFrameOffset.y * g_cursorSensitivity);
        g_mainCamera.Pitch = glm::clamp(g_mainCamera.Pitch, -glm::half_pi<float>() + 1e-4f, glm::half_pi<float>() - 1e-4f);    
    }

    g_cursorFrameOffset = {0.0, 0.0};
}

auto inline CreateBuffer(
    std::string_view label,
    size_t sizeInBytes,
    const void* data,
    uint32_t flags) -> uint32_t {

    uint32_t buffer = 0;
    glCreateBuffers(1, &buffer);
    SetDebugLabel(buffer, GL_BUFFER, label);
    glNamedBufferData(buffer, sizeInBytes, data, flags);
    return buffer;
}

auto NodeToMatrix4x4(const fastgltf::Node& node) -> glm::mat4 {

    glm::mat4 transform{1.0};

    if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform))
    {
        auto rotation = glm::quat{trs->rotation[3], trs->rotation[0], trs->rotation[1], trs->rotation[2]};
        auto scale = glm::vec3{trs->scale[0], trs->scale[1], trs->scale[2]};
        auto translation = glm::vec3{trs->translation[0], trs->translation[1], trs->translation[2]};

        glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

        // T * R * S
        transform = glm::scale(glm::translate(glm::mat4(1.0f), translation) * rotationMatrix, scale);
    }
    else if (auto* mat = std::get_if<fastgltf::Node::TransformMatrix>(&node.transform))
    {
        const auto& m = *mat;
        // clang-format off
        transform =
        { 
            m[0], m[1], m[2], m[3], 
            m[4], m[5], m[6], m[7], 
            m[8], m[9], m[10], m[11], 
            m[12], m[13], m[14], m[15],
        };
        // clang-format on
    }

    return transform;
}

auto GetVertices(
    const fastgltf::Asset& model, 
    const fastgltf::Primitive& primitive) -> std::vector<SVertexPositionNormalUv> {

    std::vector<glm::vec3> positions;
    auto& positionAccessor = model.accessors[primitive.findAttribute("POSITION")->second];
    positions.resize(positionAccessor.count);
    fastgltf::iterateAccessorWithIndex<glm::vec3>(model,
                                                  positionAccessor,
                                                  [&](glm::vec3 position, std::size_t index) { positions[index] = position; });

    std::vector<glm::vec3> normals;
    auto& normalAccessor = model.accessors[primitive.findAttribute("NORMAL")->second];
    normals.resize(normalAccessor.count);
    fastgltf::iterateAccessorWithIndex<glm::vec3>(model,
                                                  normalAccessor,
                                                  [&](glm::vec3 normal, std::size_t index) { normals[index] = normal; });

    std::vector<glm::vec2> uvs;
    if (primitive.findAttribute("TEXCOORD_0") != primitive.attributes.end())
    {
        auto& uvAccessor = model.accessors[primitive.findAttribute("TEXCOORD_0")->second];
        uvs.resize(uvAccessor.count);
        fastgltf::iterateAccessorWithIndex<glm::vec2>(model,
                                                    uvAccessor,
                                                    [&](glm::vec2 uv, std::size_t index)
                                                    { uvs[index] = uv; });
    }
    else
    {
        uvs.resize(positions.size(), {});
    }

    std::vector<SVertexPositionNormalUv> vertices;
    vertices.resize(positions.size());

    for (size_t i = 0; i < positions.size(); i++)
    {
        vertices[i] = 
        {
            positions[i],
            normals[i],
            uvs[i]
        };
    }

    return vertices;
}

auto GetIndices(
    const fastgltf::Asset& model,
    const fastgltf::Primitive& primitive) -> std::vector<uint32_t> {

    auto indices = std::vector<uint32_t>();
    auto& accessor = model.accessors[primitive.indicesAccessor.value()];
    indices.resize(accessor.count);
    fastgltf::iterateAccessorWithIndex<uint32_t>(model, accessor, [&](uint32_t value, size_t index)
    {
        indices[index] = value;
    });
    return indices;
}

auto AddModelFromFile(
    const std::string& modelName,
    std::filesystem::path filePath,
    const uint32_t megaVertexBuffer,
    const uint32_t megaIndexBuffer,
    std::unordered_map<std::string, std::vector<std::string>>& modelToPrimitiveMap,
    std::unordered_map<std::string, SMesh>& primitiveToMeshMap,
    std::unordered_map<std::string, glm::mat4x4>& primitiveToInitialTransformMap) -> void {

    if (modelToPrimitiveMap.contains(modelName))
    {
        return;
    }

    auto& meshNames = modelToPrimitiveMap[modelName];

    fastgltf::Parser parser(fastgltf::Extensions::KHR_mesh_quantization);

    auto path = std::filesystem::path{filePath};

    constexpr auto gltfOptions =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadGLBBuffers |
        fastgltf::Options::LoadExternalBuffers |
        fastgltf::Options::LoadExternalImages;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(path);

    auto assetResult = parser.loadGltf(&data, path.parent_path(), gltfOptions);
    if (assetResult.error() != fastgltf::Error::None)
    {
        spdlog::error("fastgltf: Failed to load glTF: {}", fastgltf::getErrorMessage(assetResult.error()));
        return;
    }

    auto& asset = assetResult.get();

    std::stack<std::pair<const fastgltf::Node*, glm::mat4>> nodeStack;
    glm::mat4 rootTransform = glm::mat4(1.0f);

    for (auto nodeIndex : asset.scenes[0].nodeIndices)
    {
        nodeStack.emplace(&asset.nodes[nodeIndex], rootTransform);
    }
    
    while (!nodeStack.empty())
    {
        decltype(nodeStack)::value_type top = nodeStack.top();
        const auto& [node, parentGlobalTransform] = top;
        nodeStack.pop();

        glm::mat4 localTransform = NodeToMatrix4x4(*node);
        glm::mat4 globalTransform = parentGlobalTransform * localTransform;

        for (auto childNodeIndex : node->children)
        {
            nodeStack.emplace(&asset.nodes[childNodeIndex], globalTransform);
        }

        if (node->meshIndex.has_value())
        {
            for (const fastgltf::Mesh& fgMesh = asset.meshes[node->meshIndex.value()];
                 const auto& primitive : fgMesh.primitives)
            {
                if (primitiveToMeshMap.contains(fgMesh.name.data()))
                {
                    continue;
                }

                meshNames.emplace_back(fgMesh.name);

                auto vertices = GetVertices(asset, primitive);
                auto indices = GetIndices(asset, primitive);

                SMesh mesh = {
                    .VertexCount = vertices.size(),
                    .VertexOffset = g_lastVertexOffset,
                    .IndexCount = indices.size(),
                    .IndexOffset = g_lastIndexOffset
                };

                primitiveToMeshMap.emplace(fgMesh.name, mesh);

                auto verticesSizeInBytes = sizeof(SVertexPositionNormalUv) * vertices.size();
                auto indicesSizeInBytes = sizeof(uint32_t) * indices.size();

                glNamedBufferSubData(megaVertexBuffer, g_lastVertexOffset * sizeof(SVertexPositionNormalUv), verticesSizeInBytes, vertices.data());
                glNamedBufferSubData(megaIndexBuffer, g_lastIndexOffset * sizeof(uint32_t), indicesSizeInBytes, indices.data());

                g_lastVertexOffset += vertices.size();
                g_lastIndexOffset += indices.size();

                primitiveToInitialTransformMap.emplace(fgMesh.name, globalTransform);
            }
        }
    }
}

auto main() -> int32_t {

    SWindowSettings windowSettings = {
        .ResolutionWidth = 1920,
        .ResolutionHeight = 1080,
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
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

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
    glfwSetCursorPosCallback(g_window, OnCursorPosition);
    glfwSetCursorEnterCallback(g_window, OnCursorEntered);    
    glfwSetFramebufferSizeCallback(g_window, OnWindowFramebufferSizeChanged);
    glfwSetInputMode(g_window, GLFW_CURSOR, g_cursorIsActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

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

    g_imguiContext = ImGui::CreateContext();
    g_implotContext = ImPlot::CreateContext();
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glViewport(0, 0, g_framebufferSize.x, g_framebufferSize.y);

    std::vector<SVertexPositionColor> vertices = {
        SVertexPositionColor{{+0.0f, +0.5f, +0.0f}, {0.6f, 0.1f, 0.8f, 1.0f}},
        SVertexPositionColor{{-0.5f, -0.5f, +0.0f}, {0.1f, 0.8f, 0.6f, 1.0f}},
        SVertexPositionColor{{+0.5f, -0.5f, +0.0f}, {0.8f, 0.6f, 0.1f, 1.0f}},
    };
    std::vector<uint32_t> indices = {0, 1, 2};

    auto vertexBuffer = CreateBuffer("VertexBuffer", sizeof(SVertexPositionColor) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    auto indexBuffer = CreateBuffer("IndexBuffer", sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

    auto simpleProgramPipelineResult = CreateProgramPipeline(
        "SimplePipeline",
        "data/shaders/Simple.vs.glsl",
        "data/shaders/Simple.fs.glsl");
    if (!simpleProgramPipelineResult) {
        spdlog::error(simpleProgramPipelineResult.error());
        return -7;
    }

    auto simpleProgramPipeline = *simpleProgramPipelineResult;

    SGlobalUniforms globalUniforms = {
        .ProjectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(60.0f), (float)g_framebufferSize.x, (float)g_framebufferSize.x, 0.1f, 1024.0f),
        .ViewMatrix = g_mainCamera.GetViewMatrix(),
        .CameraPosition = glm::vec4(g_mainCamera.Position, 0.0f)
    };

    auto globalUniformsBuffer = CreateBuffer("GlobalUniforms", sizeof(SGlobalUniforms), &globalUniforms, GL_DYNAMIC_DRAW);

    auto CreateWorldMatrix = [](float x, float y, float z) {
        return glm::translate(glm::mat4x4(1.0f), glm::vec3(x, y, z));
    };

    uint32_t mainTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &mainTexture);
    SetDebugLabel(mainTexture, GL_TEXTURE, std::format("MainTexture_{}x{}", g_framebufferSize.x, g_framebufferSize.y));
    glTextureStorage2D(mainTexture, 1, GL_RGBA8, g_framebufferSize.x, g_framebufferSize.y);

    uint32_t mainDepthTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &mainDepthTexture);
    SetDebugLabel(mainDepthTexture, GL_TEXTURE, std::format("MainDepthTexture_{}x{}", g_framebufferSize.x, g_framebufferSize.y));
    glTextureStorage2D(mainDepthTexture, 1, GL_DEPTH32F_STENCIL8, g_framebufferSize.x, g_framebufferSize.y);

    uint32_t mainFramebuffer = 0;
    glCreateFramebuffers(1, &mainFramebuffer);
    SetDebugLabel(mainTexture, GL_FRAMEBUFFER, "MainFramebuffer");    
    glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainTexture, 0);
    glNamedFramebufferTexture(mainFramebuffer, GL_DEPTH_ATTACHMENT, mainDepthTexture, 0);

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
    glBindVertexArray(g_defaultInputLayout);

    uint32_t megaVertexBuffer = 0;
    glCreateBuffers(1, &megaVertexBuffer);
    glNamedBufferStorage(megaVertexBuffer, 1000000000, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t megaIndexBuffer = 0;
    glCreateBuffers(1, &megaIndexBuffer);
    glNamedBufferStorage(megaIndexBuffer, 768000000, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t objectBuffer = 0;
    glCreateBuffers(1, &objectBuffer);
    glNamedBufferStorage(objectBuffer, sizeof(SObject) * 16384, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t objectIndirectBuffer = 0;
    glCreateBuffers(1, &objectIndirectBuffer);
    glNamedBufferStorage(objectIndirectBuffer, sizeof(SDrawElementCommand) * 16384, nullptr, GL_DYNAMIC_STORAGE_BIT);    

    std::unordered_map<std::string, std::vector<std::string>> modelToPrimitiveMap;
    std::unordered_map<std::string, SMesh> primitiveToMeshMap;
    std::unordered_map<std::string, glm::mat4x4> primitiveToInitialTransformMap;

    AddModelFromFile(
        "SM_Complex",
        "data/default/SM_Deccer_Cubes_Textured_Complex.gltf",
        megaVertexBuffer,
        megaIndexBuffer,
        modelToPrimitiveMap,
        primitiveToMeshMap,
        primitiveToInitialTransformMap);
    AddModelFromFile(
        "SM_Complex_Embedded",
        "data/default/SM_Deccer_Cubes_Textured_Embedded.gltf",
        megaVertexBuffer,
        megaIndexBuffer,
        modelToPrimitiveMap,
        primitiveToMeshMap,
        primitiveToInitialTransformMap);

    auto& meshNames = modelToPrimitiveMap["SM_Complex"];

    for (auto meshIndex = 0; auto& meshName : meshNames) {
        auto& mesh = primitiveToMeshMap[meshName];
        auto& transform = primitiveToInitialTransformMap[meshName];

        SObject object = {
            .WorldMatrix = transform
        };
        glNamedBufferSubData(objectBuffer, sizeof(SObject) * meshIndex, sizeof(SObject), &object);

        SDrawElementCommand drawElementCommand = {
            .IndexCount = mesh.IndexCount,
            .InstanceCount = 1,
            .FirstIndex = mesh.IndexOffset,
            .BaseVertex = mesh.VertexOffset,
            .BaseInstance = 0
        };
        glNamedBufferSubData(objectIndirectBuffer, sizeof(SDrawElementCommand) * meshIndex, sizeof(SDrawElementCommand), &drawElementCommand);

        meshIndex++;
    }    

    auto isSrgbDisabled = false;
    auto isCullfaceDisabled = false;

    g_sceneViewerSize = g_framebufferSize;

    auto previousTimeInSeconds = glfwGetTime();
    auto accumulatedTimeInSeconds = 0.0;
    while (!glfwWindowShouldClose(g_window)) {

        auto currentTimeInSeconds = glfwGetTime();
        auto deltaTimeInSeconds = currentTimeInSeconds - previousTimeInSeconds;
        accumulatedTimeInSeconds += deltaTimeInSeconds;
        previousTimeInSeconds = currentTimeInSeconds;

        HandleCamera(deltaTimeInSeconds);
        globalUniforms = {
            .ProjectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(60.0f), (float)g_framebufferSize.x, (float)g_framebufferSize.y, 0.1f, 1024.0f),
            .ViewMatrix = g_mainCamera.GetViewMatrix(),
            .CameraPosition = glm::vec4(g_mainCamera.Position, 0.0f)
        };

        glNamedBufferSubData(globalUniformsBuffer, 0, sizeof(SGlobalUniforms), &globalUniforms);

        auto framebufferWasResized = false;
        if (g_isEditor) {
            auto sceneViewerSizeScaled = glm::vec2(g_sceneViewerSize) * windowSettings.ResolutionScale;
            glViewport(0, 0, sceneViewerSizeScaled.x, sceneViewerSizeScaled.y);
            if (g_sceneViewerResized) {

                glDeleteTextures(1, &mainTexture);
                glCreateTextures(GL_TEXTURE_2D, 1, &mainTexture);
                SetDebugLabel(mainTexture, GL_TEXTURE, std::format("MainTexture_{}x{}", sceneViewerSizeScaled.x, sceneViewerSizeScaled.y));
                glTextureStorage2D(mainTexture, 1, GL_SRGB8, sceneViewerSizeScaled.x, sceneViewerSizeScaled.y);            

                glDeleteTextures(1, &mainDepthTexture);
                glCreateTextures(GL_TEXTURE_2D, 1, &mainDepthTexture);
                SetDebugLabel(mainDepthTexture, GL_TEXTURE, std::format("MainDepthTexture_{}x{}", sceneViewerSizeScaled.x, sceneViewerSizeScaled.y));
                glTextureStorage2D(mainDepthTexture, 1, GL_DEPTH32F_STENCIL8, sceneViewerSizeScaled.x, sceneViewerSizeScaled.y);

                glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainTexture, 0);
                glNamedFramebufferTexture(mainFramebuffer, GL_DEPTH_ATTACHMENT, mainDepthTexture, 0);

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

                glDeleteTextures(1, &mainDepthTexture);
                glCreateTextures(GL_TEXTURE_2D, 1, &mainDepthTexture);
                SetDebugLabel(mainDepthTexture, GL_TEXTURE, std::format("MainDepthTexture_{}x{}", framebufferSizeScaled.x, framebufferSizeScaled.y));
                glTextureStorage2D(mainDepthTexture, 1, GL_DEPTH_COMPONENT32F, framebufferSizeScaled.x, framebufferSizeScaled.y);

                glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainTexture, 0);
                glNamedFramebufferTexture(mainFramebuffer, GL_DEPTH_ATTACHMENT, mainDepthTexture, 0);                

                g_framebufferResized = false;
                framebufferWasResized = true;
            }
        }

        if (isSrgbDisabled) {
            glEnable(GL_FRAMEBUFFER_SRGB);
            isSrgbDisabled = false;
        }

        // Depth Pre Pass


        // GBuffer Pass

        PushDebugGroup("SimplePipeline");
        auto clearDepth = 1.0f;
        glClearNamedFramebufferfv(mainFramebuffer, GL_COLOR, 0, glm::value_ptr(glm::vec4{0.4f, 0.1f, 0.6f, 1.0f}));
        glClearNamedFramebufferfv(mainFramebuffer, GL_DEPTH, 0, &clearDepth);

        glBindProgramPipeline(simpleProgramPipeline);
        glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);

        //glVertexArrayElementBuffer(g_defaultInputLayout, indexBuffer);
        glVertexArrayElementBuffer(g_defaultInputLayout, megaIndexBuffer);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUniformsBuffer);
        //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, megaVertexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, objectBuffer);
        //glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr, objects.size());

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, objectIndirectBuffer);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, meshNames.size(), sizeof(SDrawElementCommand));

        PopDebugGroup();

        // UI Pass

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
            if (ImGui::SliderFloat("Resolution Scale", &resolutionScale, 0.01f, 2.0f))
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

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            if (ImGui::Begin("Scene")) {
                auto availableSceneWindowSize = ImGui::GetContentRegionAvail();
                if (availableSceneWindowSize.x != g_sceneViewerSize.x || availableSceneWindowSize.y != g_sceneViewerSize.y) {
                    g_sceneViewerSize = glm::ivec2(availableSceneWindowSize.x, availableSceneWindowSize.y);
                    g_sceneViewerResized = true;
                }
                ImGui::Image(reinterpret_cast<ImTextureID>(mainTexture), availableSceneWindowSize, g_imvec2UnitY, g_imvec2UnitX);
            }
            ImGui::PopStyleVar();
            ImGui::End();
        } else {

            PushDebugGroup("Blit To UI");
            glViewport(0, 0, g_framebufferSize.x, g_framebufferSize.y);
            DrawFullscreenTriangle(mainTexture);
            PopDebugGroup();
/*
            glBlitNamedFramebuffer(mainFramebuffer, 0,
                                   0, 0, g_framebufferSize.x, g_framebufferSize.y,
                                   0, 0, g_framebufferSize.x, g_framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);            
*/                                   
        }

        ImGui::Render();
        auto* drawData = ImGui::GetDrawData();
        if (drawData != nullptr) {
            glDisable(GL_FRAMEBUFFER_SRGB);
            isSrgbDisabled = true;
            PushDebugGroup("UI");
            ImGui_ImplOpenGL3_RenderDrawData(drawData);
            PopDebugGroup();            
        }

        glfwSwapBuffers(g_window);
        glfwPollEvents();        
    }

    glDeleteSamplers(1, &g_samplerNearestNearestClampToEdge);
    glDeleteTextures(1, &mainTexture);
    glDeleteFramebuffers(1, &mainFramebuffer);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &objectBuffer);

    glDeleteVertexArrays(1, &g_defaultInputLayout);

    glDeleteProgramPipelines(1, &simpleProgramPipeline);
    glDeleteProgramPipelines(1, &g_fullscreenTrianglePipeline);

    if (g_implotContext != nullptr) {
        ImPlot::DestroyContext(g_implotContext);
    }

    if (g_imguiContext != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(g_imguiContext);
    }
    
    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}
