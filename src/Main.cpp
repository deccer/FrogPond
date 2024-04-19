#include <algorithm>
#include <chrono>
#include <cstdint>
#include <execution>
#include <expected>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <sstream>
#include <stack>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "Macros.hpp"
#include "Io.hpp"
#include "Format.hpp"
#include "Framebuffer.hpp"
#include "DebugLabel.hpp"

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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

#include <debugbreak.h>
#if defined(TOADWART_ENABLE_LILYPAD)
#include "Lilypad.hpp"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define POOLSTL_STD_SUPPLEMENT
#include <poolstl/poolstl.hpp>

#include "ImGuiThemes.hpp"

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
/*
    glm::mat4 OldProjectionMatrix;
    glm::mat4 OldViewMatrix;
    glm::mat4 OldViewProjectionMatrix;
    */
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    /*
    glm::mat4 ViewProjectionMatrix;
    */
    glm::vec4 CameraPosition;
    /*
    glm::vec4 FrustumPlanes[6];
    glm::vec4 Viewport;
*/
};

struct SShadingUniforms {
    glm::vec4 SunDirection;
    glm::vec4 SunStrength;
};

struct SObject {
    glm::mat4x4 WorldMatrix;
    glm::ivec4 InstanceParameter;
};

struct SCpuMaterial {
    std::string Name;
    std::optional<size_t> BaseTextureIndex;
    std::optional<size_t> NormalTextureIndex;
    std::optional<size_t> OcclusionTextureIndex;
    std::optional<size_t> MetallicRoughnessTextureIndex;
    std::optional<size_t> EmissiveTextureIndex;

    size_t _padding1;

    glm::vec4 BaseColor;
};

struct SGpuMaterial {
    glm::vec4 BaseColor;

    uint64_t BaseTextureHandle;
    uint64_t NormalTextureHandle;
    uint64_t OcclusionTextureHandle;
    uint64_t MetallicRoughnessTextureHandle;

    uint64_t EmissiveTextureHandle;
    uint64_t _padding1;
};

struct alignas(4) SDebugOptions {
    int ShowMaterialId;
};

struct SGpuPooledPrimitive {
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

struct SImageData {
    int32_t Width = 0;
    int32_t Height = 0;
    int32_t PixelType = GL_UNSIGNED_BYTE;
    int32_t Bits = 8;
    int32_t Components = 0;
    std::string Name;

    std::unique_ptr<std::byte[]> EncodedData = {};
    std::size_t EncodedDataSize = 0;

    std::unique_ptr<unsigned char[]> Data = {};

    uint32_t Index = 0;
};

struct SSamplerData {
    uint64_t Name;
    uint32_t MinFilter;
    uint32_t MagFilter;
    uint32_t WrapS;
    uint32_t WrapT;
};

struct SCpuPooledMaterial {
    size_t MaterialIndex;
};

struct SCpuPooledPrimitive {
    size_t VertexCount;
    size_t VertexOffset;
    size_t IndexCount;
    size_t IndexOffset;
};

struct SPrimitive {
    SCpuPooledPrimitive Primitive;
    SCpuPooledMaterial Material;
};

struct SModelMesh {
    std::string Name;
    glm::mat4 WorldMatrix;
    std::vector<SPrimitive> Primitives;
};

struct SModel {
    std::string Name;
    std::vector<SModelMesh> Meshes;
};

constexpr ImVec2 g_imvec2UnitX = ImVec2(1, 0);
constexpr ImVec2 g_imvec2UnitY = ImVec2(0, 1);

bool g_isRunningInRenderDoc = false;
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
uint32_t g_fullscreenSamplerNearestNearestClampToEdge = 0;

SCamera g_mainCamera = {};
glm::dvec2 g_cursorPosition = {};
glm::dvec2 g_cursorFrameOffset = {};
float g_cursorSensitivity = 0.0025f;
float g_cameraSpeed = 4.0f;

bool g_cursorIsActive = true;
bool g_cursorJustEntered = false;

uint32_t g_lastVertexOffset = 0;
uint32_t g_lastIndexOffset = 0;

SDebugOptions g_debugOptions = {};
bool g_debugShowMaterialId = false;

std::unordered_map<std::string, SModel> g_modelNameToModelMap;
std::unordered_map<std::string, SCpuPooledPrimitive> g_primitiveToMeshMap;
std::unordered_map<std::string, glm::mat4x4> g_primitiveToInitialTransformMap;

std::unordered_map<size_t, std::vector<SCpuPooledPrimitive>> g_meshIndexToPrimitivesMap;

std::unordered_map<std::string, size_t> g_primitiveNameToMaterialIdMap;
std::vector<SGpuMaterial> g_gpuMaterials;
std::vector<SCpuMaterial> g_cpuMaterials;
std::vector<uint32_t> g_textures;
std::vector<uint64_t> g_textureHandles;
std::unordered_map<uint32_t, size_t> g_samplerNameToSamplerIndexMap;
std::vector<uint32_t> g_samplers;

float g_sunElevation = 3.0f;
float g_sunAzimuth = 0.3f;
glm::vec3 g_sunColor = glm::vec3{1.0f, 1.0f, 1.0f};
float g_sunStrength = 1.0f;

uint32_t g_iconPackage = 0;
uint32_t g_iconPackageGreen = 0;

bool g_gpuMaterialsNeedUpdate = true;

auto CreateProgram(
    const uint32_t shaderType,
    const std::string_view filePath,
    const std::string_view label) -> std::expected<uint32_t, std::string> {

    const auto shaderSource = ReadTextFromFile(filePath);
    if (shaderSource.empty()) {
        return std::unexpected(std::format("Either file {} was not found or is empty", filePath));
    }
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

auto CreateGraphicsProgramPipeline(
    const std::string_view label,
    const uint32_t vertexShader,
    const uint32_t fragmentShader) -> uint32_t {

    uint32_t programPipeline = 0;
    glCreateProgramPipelines(1, &programPipeline);
    SetDebugLabel(programPipeline, GL_PROGRAM_PIPELINE, label);
    glUseProgramStages(programPipeline, GL_VERTEX_SHADER_BIT, vertexShader);
    glUseProgramStages(programPipeline, GL_FRAGMENT_SHADER_BIT, fragmentShader);

    return programPipeline;
}

auto CreateComputeProgramPipeline(
    const std::string_view label,
    const uint32_t computeShader) -> uint32_t {

    uint32_t programPipeline = 0;
    glCreateProgramPipelines(1, &programPipeline);
    SetDebugLabel(GL_PROGRAM_PIPELINE, programPipeline, label);
    glUseProgramStages(programPipeline, GL_COMPUTE_SHADER_BIT, computeShader);

    return programPipeline;
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
        debug_break();
    }
}

auto GetOrCreateSampler(SSamplerData samplerData) -> uint32_t {
    if (g_samplerNameToSamplerIndexMap.contains(samplerData.Name)) {
        return g_samplers[g_samplerNameToSamplerIndexMap[samplerData.Name]];
    }

    uint32_t sampler = 0;
    glCreateSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, samplerData.WrapS);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, samplerData.WrapT);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, samplerData.MagFilter);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, samplerData.MinFilter);
    glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, 0.0f);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_LOD, -1000);
    glSamplerParameteri(sampler, GL_TEXTURE_MAX_LOD, 1000);

    g_samplers.push_back(sampler);
    g_samplerNameToSamplerIndexMap.insert({samplerData.Name, g_samplers.size() - 1});

    return sampler;
}

auto DrawFullscreenTriangleWithTexture(uint32_t texture) -> void {
    
    glBindProgramPipeline(g_fullscreenTrianglePipeline);
    glBindTextureUnit(0, texture);
    glBindSampler(0, g_fullscreenSamplerNearestNearestClampToEdge);
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

    if (!g_cursorIsActive) {

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

auto GetLocalTransform(const fastgltf::Node& node) -> glm::mat4 {

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
        transform = glm::make_mat4x4(m.data());
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

auto CalculateMipmapLevels(int32_t width, int32_t height) -> int32_t {
    return 1 + floor(log2(glm::max(width, height)));
}

auto CreateImageData(
    const void* data, 
    std::size_t dataSize, 
    fastgltf::MimeType mimeType,
    std::string_view name) -> SImageData {

    auto dataCopy = std::make_unique<std::byte[]>(dataSize);
    std::copy_n(static_cast<const std::byte*>(data), dataSize, dataCopy.get());

    return SImageData {
        .Name = std::string(name),
        .EncodedData = std::move(dataCopy),
        .EncodedDataSize = dataSize,
    };
}

auto BitfieldExtract(int32_t a, int32_t b, int32_t c) -> int32_t
{
  int mask = ~(0xffffffff << c);
  if (b > 0)
    return (a >> (b - 1)) & mask;
  else
    return a & mask;
}

auto GetPooledMaterial(
    uint32_t megaMaterialBuffer,
    uint32_t materialIndex) -> SCpuPooledMaterial {
    
    SCpuPooledMaterial pooledMaterial {
        .MaterialIndex = materialIndex
    };
    return std::move(pooledMaterial);
}

auto GetPooledPrimitive(
    uint32_t megaVertexBuffer,
    uint32_t megaIndexBuffer,
    const fastgltf::Asset& fgAsset,
    const fastgltf::Primitive& fgPrimitive) -> SCpuPooledPrimitive {

    auto vertices = GetVertices(fgAsset, fgPrimitive);
    auto indices = GetIndices(fgAsset, fgPrimitive);

    SCpuPooledPrimitive pooledPrimitive = {
        .VertexCount = vertices.size(),
        .VertexOffset = g_lastVertexOffset,
        .IndexCount = indices.size(),
        .IndexOffset = g_lastIndexOffset,
    };

    auto verticesSizeInBytes = sizeof(SVertexPositionNormalUv) * vertices.size();
    auto indicesSizeInBytes = sizeof(uint32_t) * indices.size();

    glNamedBufferSubData(megaVertexBuffer, g_lastVertexOffset * sizeof(SVertexPositionNormalUv), verticesSizeInBytes, vertices.data());
    glNamedBufferSubData(megaIndexBuffer, g_lastIndexOffset * sizeof(uint32_t), indicesSizeInBytes, indices.data());

    g_lastVertexOffset += vertices.size();
    g_lastIndexOffset += indices.size();

    return std::move(pooledPrimitive);
}

inline auto PolarToCartesian(float elevation, float azimuth) -> glm::vec3 {
    return {
        std::sin(elevation) * std::cos(azimuth), 
        std::cos(elevation),
        std::sin(elevation) * std::sin(azimuth)
    };
}

auto AddModelFromFile(
    const std::string& modelName,
    std::filesystem::path filePath,
    const uint32_t megaVertexBuffer,
    const uint32_t megaIndexBuffer,
    const uint32_t megaMaterialBuffer) -> void {

    TOADWART_PROFILE_SCOPED();
    if (g_modelNameToModelMap.contains(modelName))
    {
        return;
    }

    SModel model = {};
    
    fastgltf::Parser parser(fastgltf::Extensions::KHR_mesh_quantization);

    constexpr auto gltfOptions =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadGLBBuffers |
        fastgltf::Options::LoadExternalBuffers
        /*
         |
        fastgltf::Options::LoadExternalImages*/;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(filePath);

    auto assetResult = parser.loadGltf(&data, filePath.parent_path(), gltfOptions);
    if (assetResult.error() != fastgltf::Error::None)
    {
        spdlog::error("fastgltf: Failed to load glTF: {}", fastgltf::getErrorMessage(assetResult.error()));
        return;
    }

    auto& fgAsset = assetResult.get();

    auto imageDates = std::vector<SImageData>(fgAsset.images.size());
    const auto imageIndices = std::ranges::iota_view{(std::size_t)0, fgAsset.images.size()};

    std::transform(poolstl::execution::par, imageIndices.begin(), imageIndices.end(), imageDates.begin(), [&](size_t imageIndex) {

        TOADWART_PROFILE_NAMED_SCOPE("Load Image");
        const auto& fgImage = fgAsset.images[imageIndex];
        TOADWART_PROFILE_NAMED_SIZED_SCOPE(fgImage.name.c_str(), fgImage.name.size());

        auto imageData = [&] {

            if (const auto* filePathUri = std::get_if<fastgltf::sources::URI>(&fgImage.data)) {

                auto filePathFixed = std::filesystem::path(filePathUri->uri.path());
                auto filePathParent = filePath.parent_path();
                if (filePathFixed.is_relative()) {
                    filePathFixed = filePath.parent_path() / filePathFixed;
                }
                auto fileData = ReadBinaryFromFile(filePathFixed);
                return CreateImageData(fileData.first.get(), fileData.second, filePathUri->mimeType, fgImage.name);
            }
            if (const auto* vector = std::get_if<fastgltf::sources::Array>(&fgImage.data)) {

                return CreateImageData(vector->bytes.data(), vector->bytes.size(), vector->mimeType, fgImage.name);
            }
            if (const auto* vector = std::get_if<fastgltf::sources::Vector>(&fgImage.data)) {

                return CreateImageData(vector->bytes.data(), vector->bytes.size(), vector->mimeType, fgImage.name);
            }
            if (const auto* view = std::get_if<fastgltf::sources::BufferView>(&fgImage.data)) {

                auto& bufferView = fgAsset.bufferViews[view->bufferViewIndex];
                auto& buffer = fgAsset.buffers[bufferView.bufferIndex];
                if (const auto* vector = std::get_if<fastgltf::sources::Array>(&buffer.data)) {
                    return CreateImageData(
                        vector->bytes.data() + bufferView.byteOffset, 
                        buffer.byteLength,
                        view->mimeType,
                        fgImage.name);
                }

            }
            
            return SImageData{};
        }();

        //spdlog::info("Trying to load image {}", fgImage.name);

        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        auto* pixels = stbi_load_from_memory(
            reinterpret_cast<const unsigned char*>(imageData.EncodedData.get()),
            static_cast<int32_t>(imageData.EncodedDataSize),
            &width,
            &height,
            &components, 4);
        
        imageData.Width = width;
        imageData.Height = height;
        imageData.Components = components;
        imageData.Data.reset(pixels);
        imageData.Index = static_cast<uint32_t>(imageIndex);

        return imageData;
    });

    auto samplerDates = std::vector<SSamplerData>(fgAsset.samplers.size());
    const auto samplerIndices = std::ranges::iota_view{(std::size_t)0, fgAsset.samplers.size()};
    std::transform(poolstl::execution::par, samplerIndices.begin(), samplerIndices.end(), samplerDates.begin(), [&](size_t samplerIndex) {

        const fastgltf::Sampler& fgSampler = fgAsset.samplers[samplerIndex];

        auto hash1 = std::hash<uint32_t>()(fgSampler.minFilter.has_value() ? static_cast<uint32_t>(fgSampler.minFilter.value()) : GL_NEAREST);
        auto hash2 = std::hash<uint32_t>()(fgSampler.magFilter.has_value() ? static_cast<uint32_t>(fgSampler.magFilter.value()) : GL_NEAREST);
        auto hash3 = std::hash<uint32_t>()(static_cast<uint32_t>(fgSampler.wrapS));
        auto hash4 = std::hash<uint32_t>()(static_cast<uint32_t>(fgSampler.wrapT));
        auto hash = hash1 ^ (hash2 << 1) ^ (hash3 << 2) ^ (hash4 << 3);
        
        return SSamplerData{
            .Name = hash,
            .MinFilter = fgSampler.minFilter.has_value() ? static_cast<uint32_t>(fgSampler.minFilter.value()) : GL_NEAREST,
            .MagFilter = fgSampler.magFilter.has_value() ? static_cast<uint32_t>(fgSampler.magFilter.value()) : GL_NEAREST,
            .WrapS = static_cast<uint32_t>(fgSampler.wrapS),
            .WrapT = static_cast<uint32_t>(fgSampler.wrapT)
        };
    });    

    for (auto textureIndex = 0; auto& fgTexture : fgAsset.textures) {

        auto imageIndex = fgTexture.imageIndex.has_value() ? fgTexture.imageIndex.value() : 0;
        auto& imageData = imageDates[imageIndex];

        TOADWART_PROFILE_NAMED_SCOPE("Create Textures");
        TOADWART_PROFILE_NAMED_SIZED_SCOPE(imageData.Name.c_str(), imageData.Name.size());

        if (imageData.Data == nullptr) {
            continue;
        }

        auto samplerIndex = fgTexture.samplerIndex.has_value() ? fgTexture.samplerIndex.value() : 0;        
        auto& samplerData = samplerDates[samplerIndex];

        auto sampler = GetOrCreateSampler(samplerData);

        uint32_t textureId = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
        SetDebugLabel(textureId, GL_TEXTURE, std::to_string(textureId));
        glTextureStorage2D(textureId, CalculateMipmapLevels(imageData.Width, imageData.Height), GL_SRGB8_ALPHA8, imageData.Width, imageData.Height);
        glTextureSubImage2D(textureId, 0, 0, 0, imageData.Width, imageData.Height, GL_RGBA, imageData.PixelType, imageData.Data.get());
        glGenerateTextureMipmap(textureId);
        imageData.Data.release();

        if (g_isRunningInRenderDoc) {
            g_textures.push_back(textureId);
            g_textureHandles.push_back(textureId);

        } else {
            auto textureHandle = glGetTextureSamplerHandleARB(textureId, sampler);
            glMakeTextureHandleResidentARB(textureHandle);

            g_textures.push_back(textureId);
            g_textureHandles.push_back(textureHandle);
        }
    }

    for (auto& fgMaterial : fgAsset.materials) {

        SCpuMaterial cpuMaterial;

        cpuMaterial.Name = fgMaterial.name;

        cpuMaterial.BaseColor = glm::make_vec4(fgMaterial.pbrData.baseColorFactor.data());
        if (fgMaterial.pbrData.baseColorTexture.has_value()) {
            cpuMaterial.BaseTextureIndex = std::move(fgMaterial.pbrData.baseColorTexture.value().textureIndex);
        }
        if (fgMaterial.normalTexture.has_value()) {
            cpuMaterial.NormalTextureIndex = std::move(fgMaterial.normalTexture.value().textureIndex);
        }
        if (fgMaterial.occlusionTexture.has_value()) {
            cpuMaterial.OcclusionTextureIndex = std::move(fgMaterial.occlusionTexture.value().textureIndex);
        }
        if (fgMaterial.pbrData.metallicRoughnessTexture.has_value()) {
            cpuMaterial.MetallicRoughnessTextureIndex = std::move(fgMaterial.pbrData.metallicRoughnessTexture.value().textureIndex);
        }
        if (fgMaterial.emissiveTexture.has_value()) {
            cpuMaterial.EmissiveTextureIndex = std::move(fgMaterial.emissiveTexture.value().textureIndex);
        }

        g_cpuMaterials.push_back(cpuMaterial);
    }

    std::stack<std::pair<const fastgltf::Node*, glm::mat4>> nodeStack;
    glm::mat4 rootTransform = glm::mat4(1.0f);

    model.Name = filePath.string();

    for (auto nodeIndex : fgAsset.scenes[0].nodeIndices)
    {
        nodeStack.emplace(&fgAsset.nodes[nodeIndex], rootTransform);
    }
   
    while (!nodeStack.empty())
    {
        decltype(nodeStack)::value_type top = nodeStack.top();
        const auto& [fgNode, parentGlobalTransform] = top;
        nodeStack.pop();

        glm::mat4 localTransform = GetLocalTransform(*fgNode);
        glm::mat4 globalTransform = parentGlobalTransform * localTransform;

        for (auto childNodeIndex : fgNode->children)
        {
            nodeStack.emplace(&fgAsset.nodes[childNodeIndex], globalTransform);
        }

        if (!fgNode->meshIndex.has_value()) {
            continue;
        }

        auto meshIndex = fgNode->meshIndex.value();
        auto& fgMesh = fgAsset.meshes[meshIndex];
        auto meshName = std::string(fgMesh.name);

        auto modelMesh = SModelMesh{
            .Name = std::move(meshName),
            .WorldMatrix = std::move(globalTransform)
        };

        for (const auto& fgPrimitive : fgMesh.primitives)
        {
            TOADWART_PROFILE_NAMED_SCOPE("LoadPrimitive");
            TOADWART_PROFILE_NAMED_SIZED_SCOPE(fgMesh.name.data(), fgMesh.name.size());

            const auto primitiveMaterialIndex = fgPrimitive.materialIndex.has_value()
                ? fgPrimitive.materialIndex.value()
                : 0;

            auto pooledPrimitive = GetPooledPrimitive(
                megaVertexBuffer,
                megaIndexBuffer,
                fgAsset,
                fgPrimitive);
            auto pooledMaterial = GetPooledMaterial(
                megaMaterialBuffer,
                primitiveMaterialIndex
            );
            auto primitive = SPrimitive{
                .Primitive = std::move(pooledPrimitive),
                .Material = std::move(pooledMaterial)
            };
            modelMesh.Primitives.push_back(std::move(primitive));
        }

        model.Meshes.push_back(std::move(modelMesh));
    }

    g_modelNameToModelMap.insert({modelName, std::move(model)});
}

auto main(
    [[maybe_unused]] int32_t argc,
    [[maybe_unused]] char* argv[],
    char** environmentVariables) -> int32_t {

    g_isRunningInRenderDoc = getenv("RENDERDOC_CAPFILE") != nullptr ||
        getenv("RENDERDOC_CAPOPTS") != nullptr ||
        getenv("RENDERDOC_DEBUG_LOG_FILE") != nullptr ||
        getenv("RENDERDOC_ORIGLIBPATH") != nullptr ||
        getenv("RENDERDOC_ORIGPRELOAD") != nullptr;

    spdlog::info("Running in RenderDoc: {}", g_isRunningInRenderDoc);

    TOADWART_PROFILE_SCOPED();
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

#if defined(TOADWART_ENABLE_LILYPAD)
    if (!LoadLilypad()) {
        return -1;
    }

    SGpuInformation gpuInformation = {};
    if (!UpdateGpuInformation(0, &gpuInformation)) {
        return -1;
    }
#endif

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

    g_window = glfwCreateWindow(windowWidth, windowHeight, "Toadwart", monitor, nullptr);
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

#if defined(TOADWART_ENABLE_PROFILER)
    TracyGpuContext;
#endif

    int32_t iconWidth = 0;
    int32_t iconHeight = 0;
    int32_t iconComponents = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &g_iconPackage);
    glTextureStorage2D(g_iconPackage, 1, GL_RGBA8, 16, 16);
    auto iconFile = fopen("data/icons/package.png", "rb");
    auto iconPixels = stbi_load_from_file(iconFile, &iconWidth, &iconHeight, &iconComponents, 4);
    if (iconPixels != nullptr) {
        glTextureSubImage2D(g_iconPackage, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, iconPixels);
        stbi_image_free(iconPixels);
    }
    fclose(iconFile);

    glCreateTextures(GL_TEXTURE_2D, 1, &g_iconPackageGreen);
    glTextureStorage2D(g_iconPackageGreen, 1, GL_RGBA8, 16, 16);
    iconFile = fopen("data/icons/package_green.png", "rb");
    iconPixels = stbi_load_from_file(iconFile, &iconWidth, &iconHeight, &iconComponents, 4);
    if (iconPixels != nullptr) {
        glTextureSubImage2D(g_iconPackageGreen, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, iconPixels);
        stbi_image_free(iconPixels);
    }
    fclose(iconFile);

    g_imguiContext = ImGui::CreateContext();
    g_implotContext = ImPlot::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

/*
    io.Fonts->ClearFonts();
    io.Fonts->AddFontFromFileTTF("data/fonts/RobotoCondensed-Light.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("data/fonts/segoeui.ttf", 18.0f);
    io.Fonts->Build();
*/

    ImGui::StyleColorsDark();
    //SetGreenColorStyle();
    //SetGrayColorStyle();
    //SetDarkColorStyle();
    //SetDarkerColorStyle();
    //SetLightGrayColorStyle();
    //SetLightColorStyle();
    //SetJanekbDarknessColorStyle();
    //SetIntelRealSenseColorStyle();
    //SetAdobeSpectrumDarkColorStyle();
    //SetAdobeSpectrumLightColorStyle();
    SetDraculaColorStyle();
    //SetBrightWhiteColorStyle();
    //SetPurpleDertsehaColorStyle();
    //SetInochiDarkColorStyle();
    //SetHedgeGIColorStyle();

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
    //glDepthFunc(GL_GREATER);
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

    glViewport(0, 0, g_framebufferSize.x, g_framebufferSize.y);

    auto simpleVertexShaderResult = CreateProgram(GL_VERTEX_SHADER, "data/shaders/Simple.vs.glsl", "Simple.vs.glsl");
    if (!simpleVertexShaderResult) {
        spdlog::error(simpleVertexShaderResult.error());
        return -7;
    }
    auto simpleVertexShader = *simpleVertexShaderResult;

    auto simpleFragmentShaderResult = CreateProgram(GL_FRAGMENT_SHADER, "data/shaders/Simple.fs.glsl", "Simple.fs.glsl");
    if (!simpleFragmentShaderResult) {
        spdlog::error(simpleFragmentShaderResult.error());
        return -7;
    }
    auto simpleFragmentShader = *simpleFragmentShaderResult;

    auto simpleDebugFragmentShaderResult = CreateProgram(GL_FRAGMENT_SHADER, "data/shaders/Simple.Debug.fs.glsl", "Simple.Debug.fs.glsl");
    if (!simpleDebugFragmentShaderResult) {
        spdlog::error(simpleDebugFragmentShaderResult.error());
        return -7;
    }
    auto simpleDebugFragmentShader = *simpleDebugFragmentShaderResult;

    auto fullscreenTriangleVertexShaderResult = CreateProgram(GL_VERTEX_SHADER, "data/shaders/FST.vs.glsl", "FST.vs.glsl");
    if (!fullscreenTriangleVertexShaderResult) {
        spdlog::error(fullscreenTriangleVertexShaderResult.error());
        return -7;
    }
    auto fullscreenTriangleVertexShader = *fullscreenTriangleVertexShaderResult;

    auto fullscreenTriangleFragmentShaderResult = CreateProgram(GL_FRAGMENT_SHADER, "data/shaders/FST.fs.glsl", "FST.fs.glsl");
    if (!fullscreenTriangleFragmentShaderResult) {
        spdlog::error(fullscreenTriangleFragmentShaderResult.error());
        return -7;
    }
    auto fullscreenTriangleFragmentShader = *fullscreenTriangleFragmentShaderResult;

    auto simpleProgramPipeline = CreateGraphicsProgramPipeline("SimplePipeline", simpleVertexShader, simpleFragmentShader);
    auto simpleDebugProgramPipeline = CreateGraphicsProgramPipeline("SimpleDebugPipeline", simpleVertexShader, simpleDebugFragmentShader);
    g_fullscreenTrianglePipeline = CreateGraphicsProgramPipeline("FST", fullscreenTriangleVertexShader, fullscreenTriangleFragmentShader);

    auto shadowVertexShaderResult = CreateProgram(GL_VERTEX_SHADER, "data/shaders/Shadow.vs.glsl", "Shadow.vs.glsl");
    if (!shadowVertexShaderResult) {
        spdlog::error(shadowVertexShaderResult.error());
        return -7;
    }
    auto shadowVertexShader = *shadowVertexShaderResult;

    auto shadowFragmentShaderResult = CreateProgram(GL_FRAGMENT_SHADER, "data/shaders/Shadow.fs.glsl", "Shadow.fs.glsl");
    if (!shadowFragmentShaderResult) {
        spdlog::error(shadowFragmentShaderResult.error());
        return -7;
    }
    auto shadowFragmentShader = *shadowFragmentShaderResult;
    auto shadowProgramPipeline = CreateGraphicsProgramPipeline("Shadow", shadowVertexShader, shadowFragmentShader);

    SGlobalUniforms globalUniforms = {
        .ProjectionMatrix = glm::infinitePerspectiveRH_ZO(glm::radians(60.0f), (float)g_framebufferSize.x / (float)g_framebufferSize.x, 0.1f),
        //.ProjectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(60.0f), (float)g_framebufferSize.x, (float)g_framebufferSize.x, 0.1f, 1024.0f),
        .ViewMatrix = g_mainCamera.GetViewMatrix(),
        .CameraPosition = glm::vec4(g_mainCamera.Position, 0.0f),
    };

    auto globalUniformsBuffer = CreateBuffer("GlobalUniforms", sizeof(SGlobalUniforms), &globalUniforms, GL_DYNAMIC_DRAW);

    SShadingUniforms shadingUniforms = {
        .SunDirection = glm::vec4(PolarToCartesian(g_sunElevation, g_sunAzimuth), 0),
        .SunStrength = glm::vec4{g_sunStrength * g_sunColor, 0}
    };
    auto shadingUniformsBuffer = CreateBuffer("ShadingUniforms", sizeof(SShadingUniforms), &shadingUniforms, GL_DYNAMIC_DRAW);

    auto CreateWorldMatrix = [](float x, float y, float z) {
        return glm::translate(glm::mat4x4(1.0f), glm::vec3(x, y, z));
    };

    SFramebufferAttachmentDescriptor mainFramebufferAttachmentDescriptors[] = { 
        { EFormat::R8G8B8A8_Srgb, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f) }, 
        { EFormat::R32G32B32A32_Float, std::nullopt }, 
        { EFormat::D32_Float, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)},
    };
    auto mainFramebuffer = CreateFramebuffer("MainFramebuffer", g_framebufferSize.x, g_framebufferSize.y, mainFramebufferAttachmentDescriptors);

    SFramebufferAttachmentDescriptor shadowFramebufferAttachmentDescriptors[] = { 
        { EFormat::D32_Float, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)},
    };
    auto shadowFramebuffer = CreateFramebuffer("ShadowFramebuffer", 1024, 1024,  shadowFramebufferAttachmentDescriptors);

    g_fullscreenSamplerNearestNearestClampToEdge = GetOrCreateSampler(SSamplerData{
        .Name = 0,
        .MinFilter = GL_NEAREST,        
        .MagFilter = GL_NEAREST,
        .WrapS = GL_REPEAT,
        .WrapT = GL_REPEAT,
    });
    glCreateVertexArrays(1, &g_defaultInputLayout);
    SetDebugLabel(g_defaultInputLayout, GL_VERTEX_ARRAY, "InputLayout_Empty");
    glBindVertexArray(g_defaultInputLayout);

    uint32_t megaVertexBuffer = 0;
    glCreateBuffers(1, &megaVertexBuffer);
    glNamedBufferStorage(megaVertexBuffer, 1000000000, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t megaIndexBuffer = 0;
    glCreateBuffers(1, &megaIndexBuffer);
    glNamedBufferStorage(megaIndexBuffer, 768000000, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t megaMaterialBuffer = 0;
    glCreateBuffers(1, &megaMaterialBuffer);
    glNamedBufferStorage(megaMaterialBuffer, sizeof(SGpuMaterial) * 512, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t objectBuffer = 0;
    glCreateBuffers(1, &objectBuffer);
    glNamedBufferStorage(objectBuffer, sizeof(SObject) * 65536, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t objectIndirectBuffer = 0;
    glCreateBuffers(1, &objectIndirectBuffer);
    glNamedBufferStorage(objectIndirectBuffer, sizeof(SGpuPooledPrimitive) * 65536, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t gpuMaterialBuffer = 0;
    glCreateBuffers(1, &gpuMaterialBuffer);
    glNamedBufferStorage(gpuMaterialBuffer, sizeof(SGpuMaterial) * 512, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t cpuMaterialBuffer = 0;
    glCreateBuffers(1, &cpuMaterialBuffer);
    glNamedBufferStorage(cpuMaterialBuffer, sizeof(SCpuMaterial) * 512, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t debugOptionsBuffer = 0;
    glCreateBuffers(1, &debugOptionsBuffer);
    glNamedBufferStorage(debugOptionsBuffer, sizeof(SDebugOptions), nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t defaultSampler = GetOrCreateSampler({
        .Name = 0ul,
        .MinFilter = GL_NEAREST,        
        .MagFilter = GL_NEAREST,
        .WrapS = GL_REPEAT,
        .WrapT = GL_REPEAT
    });

/*
    uint32_t defaultTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &defaultTexture);
    SetDebugLabel(defaultTexture, GL_TEXTURE, "defaultTexture");
    glTextureStorage2D(defaultTexture, 1, GL_RGBA8, 2, 2);
    std::array<uint32_t, 4> defaultTexturePixels = {
        0xFF0000FF,
        0xFF00FFFF,
        0xFF00FF00,
        0xFFFFFF00
    };
    glTextureSubImage2D(defaultTexture, 0, 0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, defaultTexturePixels.data());
    glGenerateTextureMipmap(defaultTexture);

    auto defaultTextureHandle = glGetTextureSamplerHandleARB(defaultTexture, defaultSampler);
    glMakeTextureHandleResidentARB(defaultTextureHandle);

    SMaterial defaultMaterial = {
        .BaseTexture = defaultTextureHandle,
        .NormalTexture = 0,        
        .OcclusionTexture = 0,        
        .MetallicRoughnessTexture = 0,        
        .EmissiveTexture = 0,
        .BaseColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
    };
    g_materials.push_back(defaultMaterial);
    g_textures.push_back(defaultTexture);
    g_textureHandles.push_back(defaultTextureHandle);


    g_baseTextureIndex = g_textures.size();
    g_lastMaterialIndex = g_materials.size();
    */

/*
    AddModelFromFile(
        "SM_Model",
        "data/default/SM_Deccer_Cubes_Textured_Complex.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
*/
/*
    AddModelFromFile(
        "SM_Model",
        "data/default/SM_Beams_01.glb",
        megaVertexBuffer,
        megaIndexBuffer);
*/
/*
    AddModelFromFile(
        "SM_Model",
        "data/default/SM_Deccer_Cubes_Textured_Embedded.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
*/

    AddModelFromFile(
        "SM_Model",
        "data/default/SM_Deccer_Cubes_Textured.gltf",
        megaVertexBuffer,
        megaIndexBuffer,
        megaMaterialBuffer);

/*
    AddModelFromFile(
        "SM_Model",
        "data/scenes/DamagedHelmet/DamagedHelmet.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
*/
/*
    AddModelFromFile(
        "SM_Model",
        "data/scenes/Bistro52/scene.gltf",
        megaVertexBuffer,
        megaIndexBuffer,
        megaMaterialBuffer);
*/
/*
    AddModelFromFile(
        "SM_Model",
        "data/scenes/Tower/scene.gltf",
        megaVertexBuffer,
        megaIndexBuffer,
        megaMaterialBuffer);
*/      
/*
    AddModelFromFile(
        "SM_Model",
        "data/scenes/IntelSponza/NewSponza_Main_glTF_002.gltf",
        megaVertexBuffer,
        megaIndexBuffer,
        megaMaterialBuffer);
*/ 
/*
    AddModelFromFile(
        "SM_Model",
        "data/default/mira_up/scene.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
*/

    auto model = g_modelNameToModelMap["SM_Model"];

    // prepare material buffer, in this instance its update per material, cpu materials should be transformed into gpu materials
    // and gpumaterials uploaded to gpu at once, rather than one after another

    for (auto materialIndex = 0; auto& cpuMaterial : g_cpuMaterials) {

        glNamedBufferSubData(cpuMaterialBuffer, sizeof(SCpuMaterial) * materialIndex, sizeof(SGpuMaterial), &cpuMaterial);
        auto gpuMaterial = SGpuMaterial{
            .BaseColor = cpuMaterial.BaseColor,
            .BaseTextureHandle = cpuMaterial.BaseTextureIndex.has_value()
                ? g_textureHandles[cpuMaterial.BaseTextureIndex.value()]
                : 0,
            .NormalTextureHandle = cpuMaterial.NormalTextureIndex.has_value()
                ? g_textureHandles[cpuMaterial.NormalTextureIndex.value()]
                : 0,
            .OcclusionTextureHandle = cpuMaterial.OcclusionTextureIndex.has_value()
                ? g_textureHandles[cpuMaterial.OcclusionTextureIndex.value()]
                : 0,
            .MetallicRoughnessTextureHandle = cpuMaterial.MetallicRoughnessTextureIndex.has_value()
                ? g_textureHandles[cpuMaterial.MetallicRoughnessTextureIndex.value()]
                : 0,
            .EmissiveTextureHandle = cpuMaterial.EmissiveTextureIndex.has_value()
                ? g_textureHandles[cpuMaterial.EmissiveTextureIndex.value()]
                : 0,
            ._padding1 = 0,
        };
        glNamedBufferSubData(gpuMaterialBuffer, sizeof(SGpuMaterial) * materialIndex, sizeof(SGpuMaterial), &gpuMaterial);
        materialIndex++;
    }

    auto primitiveCount = 0;
    for (auto meshIndex = 0; auto mesh : model.Meshes) {

        auto transform = mesh.WorldMatrix;
        for (auto primitiveIndex = 0; auto primitive : mesh.Primitives) {

            SObject object = {
                .WorldMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)) * transform,
                .InstanceParameter = glm::ivec4(primitive.Material.MaterialIndex, 0, 0, 0)
            };
            glNamedBufferSubData(objectBuffer, sizeof(SObject) * primitiveCount, sizeof(SObject), &object);

            SGpuPooledPrimitive gpuPooledPrimitive = {
                .IndexCount = static_cast<uint32_t>(primitive.Primitive.IndexCount),
                .InstanceCount = 1,
                .FirstIndex = static_cast<uint32_t>(primitive.Primitive.IndexOffset),
                .BaseVertex = static_cast<int32_t>(primitive.Primitive.VertexOffset),
                .BaseInstance = 0
            };
            glNamedBufferSubData(objectIndirectBuffer, sizeof(SGpuPooledPrimitive) * primitiveCount, sizeof(SGpuPooledPrimitive), &gpuPooledPrimitive);
            primitiveIndex++;
            primitiveCount++;
        }

        meshIndex++;
    }

    auto isSrgbDisabled = false;
    auto isCullfaceDisabled = false;

    g_sceneViewerSize = g_framebufferSize;
    glm::vec2 scaledFramebufferSize = glm::vec2(g_sceneViewerSize) * windowSettings.ResolutionScale;

    uint64_t frameCounter = 0;

    auto previousTimeInSeconds = glfwGetTime();
    auto accumulatedTimeInSeconds = 0.0;
    while (!glfwWindowShouldClose(g_window)) {

        TOADWART_PROFILE_NAMED_SCOPE("Render");
        //TracyGpuZone("Render");

        auto currentTimeInSeconds = glfwGetTime();
        auto deltaTimeInSeconds = currentTimeInSeconds - previousTimeInSeconds;
        accumulatedTimeInSeconds += deltaTimeInSeconds;
        previousTimeInSeconds = currentTimeInSeconds;

        HandleCamera(deltaTimeInSeconds);
        globalUniforms = {
            .ProjectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(60.0f), (float)g_sceneViewerSize.x, (float)g_sceneViewerSize.y, 0.1f, 1024.0f),
            .ViewMatrix = g_mainCamera.GetViewMatrix(),
            .CameraPosition = glm::vec4(g_mainCamera.Position, 0.0f)
        };

        glNamedBufferSubData(globalUniformsBuffer, 0, sizeof(SGlobalUniforms), &globalUniforms);

        shadingUniforms = {
            .SunDirection = glm::vec4(PolarToCartesian(g_sunElevation, g_sunAzimuth), 0),
            .SunStrength = glm::vec4{g_sunStrength * g_sunColor, 0}
        };
        glNamedBufferSubData(shadingUniformsBuffer, 0, sizeof(SShadingUniforms), &shadingUniforms);

        glNamedBufferSubData(debugOptionsBuffer, 0, sizeof(SDebugOptions), &g_debugOptions);

        auto framebufferWasResized = false;


        if (g_sceneViewerResized || g_framebufferResized) {

            TOADWART_PROFILE_NAMED_SCOPE("Resize");

            if (g_isEditor) {
                scaledFramebufferSize = glm::vec2(g_sceneViewerSize) * windowSettings.ResolutionScale;
                g_sceneViewerResized = false;
            } else {
                scaledFramebufferSize = glm::vec2(g_framebufferSize) * windowSettings.ResolutionScale;
                g_framebufferResized = false;
            }

            ResizeFramebuffer(mainFramebuffer, scaledFramebufferSize.x, scaledFramebufferSize.y);

            framebufferWasResized = true;
        }

        glViewport(0, 0, scaledFramebufferSize.x, scaledFramebufferSize.y);

        if (isSrgbDisabled) {
            glEnable(GL_FRAMEBUFFER_SRGB);
            isSrgbDisabled = false;
        }

        // Depth Pre Pass

        // Shadow Pass

        PushDebugGroup("ShadowPass");

        glViewport(0, 0, shadowFramebuffer.Width, shadowFramebuffer.Height);
        glBindProgramPipeline(shadowProgramPipeline);

        PopDebugGroup();

        // GBuffer Pass

        PushDebugGroup("SimplePipeline");

        if (g_debugShowMaterialId) {
            glBindProgramPipeline(simpleDebugProgramPipeline);
        } else {
            glBindProgramPipeline(simpleProgramPipeline);
        }
        BindFramebuffer(mainFramebuffer);

        auto clearDepth = 1.0f;
        glDisable(GL_FRAMEBUFFER_SRGB);
        glColorMaski(0, true, true, true, true);
        glColorMaski(1, true, true, true, true);

        ClearFramebuffer(mainFramebuffer);
        glEnable(GL_FRAMEBUFFER_SRGB);

        glVertexArrayElementBuffer(g_defaultInputLayout, megaIndexBuffer);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUniformsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, megaVertexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, objectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, gpuMaterialBuffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, 4, shadingUniformsBuffer);

        if (g_debugShowMaterialId) {
            //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 19, cpuMaterialBuffer);
            //glBindBufferBase(GL_UNIFORM_BUFFER, 20, debugOptionsBuffer);
        }

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, objectIndirectBuffer);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, primitiveCount, sizeof(SGpuPooledPrimitive));

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

        if (!g_isEditor) {
            ImGui::SetNextWindowPos({32, 32});
#if defined(TOADWART_ENABLE_LILYPAD)            
            ImGui::SetNextWindowSize({168, 192});
#else
            ImGui::SetNextWindowSize({168, 136});
#endif
            auto windowBackgroundColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
            windowBackgroundColor.w = 0.4f;
            ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBackgroundColor);
            if (ImGui::Begin("#InGameStatistics", nullptr, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoDecoration)) {

                ImGui::TextColored(ImVec4{0.9f, 0.7f, 0.0f, 1.0f}, "F1 to toggle editor");
                ImGui::SeparatorText("Frame Statistics");

                auto framesPerSecond = 1.0f / deltaTimeInSeconds;
                ImGui::Text("afps: %.0f rad/s", glm::two_pi<float>() * framesPerSecond);
                ImGui::Text("dfps: %.0f °/s", glm::degrees(glm::two_pi<float>() * framesPerSecond));
                ImGui::Text("rfps: %.0f", framesPerSecond);
                ImGui::Text("rpms: %.0f", framesPerSecond * 60.0f);
                ImGui::Text("  ft: %.2f ms", deltaTimeInSeconds * 1000.0f);
#if defined(TOADWART_ENABLE_LILYPAD)
                ImGui::SeparatorText("GPU Statistics");
                ImGui::Text("temp: %d °C", gpuInformation.GpuCoreTemperature);
                ImGui::Text("  cs: %d (%d)/(%d) MHz", gpuInformation.ClockSpeedCurrent, gpuInformation.ClockSpeedMin, gpuInformation.ClockSpeedMax);
                ImGui::Text(" mcs: %d (%d)/(%d) MHz", gpuInformation.MemoryClockSpeed, gpuInformation.MemoryClockSpeedMin, gpuInformation.MemoryClockSpeedMax);
#endif
            }
            ImGui::End();
            ImGui::PopStyleColor();
        }

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

            ImGui::SliderFloat("Sun Azimuth", &g_sunAzimuth, -3.1415f, 3.1415f);
            ImGui::SliderFloat("Sun Elevation", &g_sunElevation, 0, 3.1415f);
            ImGui::ColorEdit3("Sun Color", &g_sunColor[0], ImGuiColorEditFlags_Float);
            ImGui::SliderFloat("Sun Strength", &g_sunStrength, 0, 500, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat);
        }
        ImGui::End();

        if (g_isEditor) {            

            static int drawMainFramebufferIndex = 0;
            if (ImGui::Begin("Debug")) {

                if (g_debugShowMaterialId) {
                    ImGui::RadioButton("MaterialId", &drawMainFramebufferIndex, 0);
                    ImGui::RadioButton("TextureHandle", &drawMainFramebufferIndex, 1);
                } else {
                    ImGui::RadioButton("Albedo", &drawMainFramebufferIndex, 0);
                    ImGui::RadioButton("Normals", &drawMainFramebufferIndex, 1);
                }

                ImGui::Separator();
                if (ImGui::Checkbox("Debug Colors", &g_debugShowMaterialId))
                {
                    g_debugOptions.ShowMaterialId = g_debugShowMaterialId ? 1 : 0;
                }
            }
            ImGui::End();

            if (!g_modelNameToModelMap.empty()) {
                if (ImGui::Begin("Assets")) {
                    if (ImGui::BeginTable("Models", 2, ImGuiTableFlags_::ImGuiTableFlags_RowBg)) {
                        
                        ImGui::TableSetupColumn("Model", ImGuiTableColumnFlags_NoSort);
                        ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 32);
                        ImGui::TableHeadersRow();

                        for (auto& modelNameToModel : g_modelNameToModelMap) {
                            ImGui::TableNextRow();
                            ImGui::PushID(static_cast<int32_t>(std::hash<std::string>{}(modelNameToModel.first)));
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Image(reinterpret_cast<ImTextureID>(g_iconPackage), ImVec2{16, 16}, g_imvec2UnitY, g_imvec2UnitX);
                            ImGui::SameLine();
                            auto isExpanded = ImGui::TreeNodeEx(modelNameToModel.first.data());

                            ImGui::TableSetColumnIndex(1);
                            if (ImGui::Button("Add")) {
                                // add model (and its primitives)
                            }

                            if (isExpanded) {
                                std::vector<SModelMesh>& meshes = modelNameToModel.second.Meshes;
                                for (auto& modelMesh : meshes) {
                                    ImGui::TableNextRow();
                                    ImGui::TableSetColumnIndex(0);
                                    ImGui::Image(reinterpret_cast<ImTextureID>(g_iconPackageGreen), ImVec2{16, 16}, g_imvec2UnitY, g_imvec2UnitX);
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(modelMesh.Name.data());
                                    ImGui::TableSetColumnIndex(1);
                                    if (ImGui::Button("Add")) {
                                        // add primitive
                                    }
                                }

                                ImGui::TreePop();
                            }

                            ImGui::PopID();
                        }

                        ImGui::EndTable();
                    }
                }
                ImGui::End();
            }

            if (!g_cpuMaterials.empty()) {
                if (ImGui::Begin("Materials")) {
                    auto textureSize = ImVec2{32, 32};
                    auto textureSizeLarge = ImVec2{512, 512};
                    for ( auto materialIndex = 0; auto& cpuMaterial : g_cpuMaterials) {
                        
                        if (ImGui::CollapsingHeader(cpuMaterial.Name.data())) {
                            if (cpuMaterial.BaseTextureIndex.has_value()) {
                                ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.BaseTextureIndex.value()]), textureSize, g_imvec2UnitY, g_imvec2UnitX);
                                if (ImGui::BeginItemTooltip())
                                {
                                    ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.BaseTextureIndex.value()]), textureSizeLarge, g_imvec2UnitY, g_imvec2UnitX);
                                    ImGui::EndTooltip();
                                }
                                ImGui::SameLine();
                                ImGui::TextUnformatted(std::format("Base Texture ({})", cpuMaterial.BaseTextureIndex.value()).c_str());
                                ImGui::Separator();
                            }

                            if (cpuMaterial.NormalTextureIndex.has_value()) {
                                ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.NormalTextureIndex.value()]), textureSize, g_imvec2UnitY, g_imvec2UnitX);
                                if (ImGui::BeginItemTooltip())
                                {
                                    ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.NormalTextureIndex.value()]), textureSizeLarge, g_imvec2UnitY, g_imvec2UnitX);
                                    ImGui::EndTooltip();
                                }
                                ImGui::SameLine();
                                ImGui::TextUnformatted(std::format("Normal Texture ({})", cpuMaterial.NormalTextureIndex.value()).c_str());
                                ImGui::Separator();
                            }

                            if (cpuMaterial.OcclusionTextureIndex.has_value()) {
                                ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.OcclusionTextureIndex.value()]), textureSize, g_imvec2UnitY, g_imvec2UnitX);
                                if (ImGui::BeginItemTooltip())
                                {
                                    ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.OcclusionTextureIndex.value()]), textureSizeLarge, g_imvec2UnitY, g_imvec2UnitX);
                                    ImGui::EndTooltip();
                                }
                                ImGui::SameLine();                                
                                ImGui::TextUnformatted(std::format("Occlusion Texture ({})", cpuMaterial.OcclusionTextureIndex.value()).c_str());
                                ImGui::Separator();
                            }

                            if (cpuMaterial.MetallicRoughnessTextureIndex.has_value()) {
                                ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.MetallicRoughnessTextureIndex.value()]), textureSize, g_imvec2UnitY, g_imvec2UnitX);
                                if (ImGui::BeginItemTooltip())
                                {
                                    ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.MetallicRoughnessTextureIndex.value()]), textureSizeLarge, g_imvec2UnitY, g_imvec2UnitX);
                                    ImGui::EndTooltip();
                                }
                                ImGui::SameLine();
                                ImGui::TextUnformatted(std::format("Metalness Roughness Texture ({})", cpuMaterial.MetallicRoughnessTextureIndex.value()).c_str());
                                ImGui::Separator();
                            }

                            if (cpuMaterial.EmissiveTextureIndex.has_value()) {
                                ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.EmissiveTextureIndex.value()]), textureSize, g_imvec2UnitY, g_imvec2UnitX);
                                if (ImGui::BeginItemTooltip())
                                {
                                    ImGui::Image(reinterpret_cast<ImTextureID>(g_textures[cpuMaterial.EmissiveTextureIndex.value()]), textureSizeLarge, g_imvec2UnitY, g_imvec2UnitX);
                                    ImGui::EndTooltip();
                                }
                                ImGui::SameLine();
                                ImGui::TextUnformatted(std::format("Emissive Texture ({})", cpuMaterial.EmissiveTextureIndex.value()).c_str());
                                ImGui::Separator();
                            }
                        }

                        materialIndex++;
                    }
                }
                ImGui::End();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            if (ImGui::Begin("Scene")) {
                auto availableSceneWindowSize = ImGui::GetContentRegionAvail();
                if (availableSceneWindowSize.x != g_sceneViewerSize.x || availableSceneWindowSize.y != g_sceneViewerSize.y) {
                    g_sceneViewerSize = glm::ivec2(availableSceneWindowSize.x, availableSceneWindowSize.y);
                    g_sceneViewerResized = true;
                }

                auto texture = drawMainFramebufferIndex == 0
                    ? mainFramebuffer.Attachments[0].AttachmentId
                    : mainFramebuffer.Attachments[1].AttachmentId;
                auto imagePosition = ImGui::GetCursorPos();
                ImGui::Image(reinterpret_cast<ImTextureID>(texture), availableSceneWindowSize, g_imvec2UnitY, g_imvec2UnitX);
                ImGui::SetCursorPos(imagePosition);
                if (ImGui::BeginChild(1, ImVec2{192, -1})) {
                    if (ImGui::CollapsingHeader("Statistics")) {
                        ImGui::Text("Text");
                    }
                }
                ImGui::EndChild();
            }
            ImGui::PopStyleVar();
            ImGui::End();

            if (ImGui::Begin("ImGui")) {
                ImGui::ShowStyleEditor(nullptr);
            }
            ImGui::End();
        } else {

            PushDebugGroup("Blit To UI");
            glViewport(0, 0, g_framebufferSize.x, g_framebufferSize.y);
            DrawFullscreenTriangleWithTexture(mainFramebuffer.Attachments[0].AttachmentId);
            PopDebugGroup();
/*
            glBlitNamedFramebuffer(mainFramebuffer, 0,
                                   0, 0, g_framebufferSize.x, g_framebufferSize.y,
                                   0, 0, g_framebufferSize.x, g_framebufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);            
*/                                   
        }

        ImGui::Render();
        auto* imGuiDrawData = ImGui::GetDrawData();
        if (imGuiDrawData != nullptr) {
            glDisable(GL_FRAMEBUFFER_SRGB);
            isSrgbDisabled = true;
            PushDebugGroup("UI");
            ImGui_ImplOpenGL3_RenderDrawData(imGuiDrawData);
            PopDebugGroup();            
        }
        {
            TOADWART_PROFILE_NAMED_SCOPE("SwapBuffers");
            glfwSwapBuffers(g_window);
        }

        glfwPollEvents();

        frameCounter++;
#if defined(TOADWART_ENABLE_LILYPAD)        
        if ((frameCounter % 2000) == 0) {
            UpdateGpuInformation(0, &gpuInformation);
        }
#endif        

        TOADWART_MARK_GPU_FRAME();
        TOADWART_MARK_FRAME();
    }

    glDeleteSamplers(1, &g_fullscreenSamplerNearestNearestClampToEdge);
    for(auto sampler : g_samplers) {
        glDeleteSamplers(1, &sampler);
    }
    for (auto textureHandle : g_textureHandles) {
        if (glIsTextureHandleResidentARB(textureHandle)) {
            glMakeTextureHandleNonResidentARB(textureHandle);
        }
    }
    for (auto texture : g_textures) {
        glDeleteTextures(1, &texture);
    }

    DestroyFramebuffer(mainFramebuffer);

    glDeleteBuffers(1, &objectBuffer);
    glDeleteBuffers(1, &megaMaterialBuffer);
    glDeleteBuffers(1, &megaVertexBuffer);
    glDeleteBuffers(1, &megaIndexBuffer);
    glDeleteBuffers(1, &cpuMaterialBuffer);
    glDeleteBuffers(1, &gpuMaterialBuffer);

    glDeleteVertexArrays(1, &g_defaultInputLayout);

    glDeleteProgram(simpleVertexShader);
    glDeleteProgram(simpleFragmentShader);
    glDeleteProgram(simpleDebugFragmentShader);
    glDeleteProgramPipelines(1, &simpleDebugProgramPipeline);
    glDeleteProgramPipelines(1, &simpleProgramPipeline);
    glDeleteProgram(fullscreenTriangleVertexShader);
    glDeleteProgram(fullscreenTriangleFragmentShader);
    glDeleteProgramPipelines(1, &g_fullscreenTrianglePipeline);
    glDeleteProgram(shadowVertexShader);
    glDeleteProgram(shadowFragmentShader);
    glDeleteProgramPipelines(1, &shadowProgramPipeline);

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
