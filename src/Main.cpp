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
#include <functional>
#include <string_view>
#include <expected>
#include <memory>
#include <utility>
#include <span>
#include <tuple>
#include <stack>
#include <variant>
#include <ranges>
#include <thread>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

#include <tracy/Tracy.hpp>

#include <debugbreak.h>
#if USE_LILLYPAD
#include "Lillypad.hpp"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <execution>
#define POOLSTL_STD_SUPPLEMENT
#include <poolstl/poolstl.hpp>

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

struct SObject {
    glm::mat4x4 WorldMatrix;
    glm::ivec4 InstanceParameter;
};

struct SMesh {
    size_t VertexCount;
    size_t VertexOffset;
    size_t IndexCount;
    size_t IndexOffset;
    uint32_t MaterialIndex;
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

std::unordered_map<std::string, std::vector<std::string>> g_modelToPrimitiveMap;
std::unordered_map<std::string, SMesh> g_primitiveToMeshMap;
std::unordered_map<std::string, glm::mat4x4> g_primitiveToInitialTransformMap;

std::unordered_map<std::string, size_t> g_primitiveNameToMaterialIdMap;
std::vector<SGpuMaterial> g_gpuMaterials;
std::vector<SCpuMaterial> g_cpuMaterials;
std::vector<uint32_t> g_textures;
std::vector<uint64_t> g_textureHandles;
std::unordered_map<uint32_t, size_t> g_samplerNameToSamplerIndexMap;
std::vector<uint32_t> g_samplers;

bool g_gpuMaterialsNeedUpdate = true;

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

auto DrawFullscreenTriangle(uint32_t texture) -> void {
    
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

auto AddModelFromFile(
    const std::string& modelName,
    std::filesystem::path filePath,
    const uint32_t megaVertexBuffer,
    const uint32_t megaIndexBuffer) -> void {

    ZoneScoped;
    if (g_modelToPrimitiveMap.contains(modelName))
    {
        return;
    }

    auto& meshNames = g_modelToPrimitiveMap[modelName];
    
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

        ZoneScopedN("Load Image");
        const auto& fgImage = fgAsset.images[imageIndex];
        ZoneName(fgImage.name.c_str(), fgImage.name.size());

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

        ZoneScopedN("Create Textures");
        ZoneName(imageData.Name.c_str(), imageData.Name.size());

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

        auto textureHandle = glGetTextureSamplerHandleARB(textureId, sampler);
        glMakeTextureHandleResidentARB(textureHandle);

        g_textures.push_back(textureId);
        g_textureHandles.push_back(textureHandle);        
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

    for (auto nodeIndex : fgAsset.scenes[0].nodeIndices)
    {
        nodeStack.emplace(&fgAsset.nodes[nodeIndex], rootTransform);
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
            nodeStack.emplace(&fgAsset.nodes[childNodeIndex], globalTransform);
        }

        if (node->meshIndex.has_value())
        {
            for (const fastgltf::Mesh& fgMesh = fgAsset.meshes[node->meshIndex.value()];
                 const auto& primitive : fgMesh.primitives)
            {
                ZoneScopedN("LoadPrimitive");
                ZoneName(fgMesh.name.c_str(), fgMesh.name.size());

                if (g_primitiveToMeshMap.contains(fgMesh.name.data()))
                {
                    continue;
                }
                std::string primitiveName = std::string(fgMesh.name);

                const auto materialIndex = primitive.materialIndex.has_value()
                    ? primitive.materialIndex.value()
                    : 0;

                meshNames.emplace_back(primitiveName);
                g_primitiveNameToMaterialIdMap.insert({primitiveName, materialIndex});

                auto vertices = GetVertices(fgAsset, primitive);
                auto indices = GetIndices(fgAsset, primitive);

                SMesh mesh = {
                    .VertexCount = vertices.size(),
                    .VertexOffset = g_lastVertexOffset,
                    .IndexCount = indices.size(),
                    .IndexOffset = g_lastIndexOffset,
                    .MaterialIndex = static_cast<uint32_t>(materialIndex),
                };

                g_primitiveToMeshMap.insert({primitiveName, mesh});

                auto verticesSizeInBytes = sizeof(SVertexPositionNormalUv) * vertices.size();
                auto indicesSizeInBytes = sizeof(uint32_t) * indices.size();

                glNamedBufferSubData(megaVertexBuffer, g_lastVertexOffset * sizeof(SVertexPositionNormalUv), verticesSizeInBytes, vertices.data());
                glNamedBufferSubData(megaIndexBuffer, g_lastIndexOffset * sizeof(uint32_t), indicesSizeInBytes, indices.data());

                g_lastVertexOffset += vertices.size();
                g_lastIndexOffset += indices.size();

                g_primitiveToInitialTransformMap.insert({primitiveName, globalTransform});
            }
        }
    }
}

auto main() -> int32_t {

    ZoneScoped;
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

#if USE_LILLYPAD
    if (!LoadLillypad()) {
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

    SGlobalUniforms globalUniforms = {
        .ProjectionMatrix = glm::infinitePerspectiveRH_ZO(glm::radians(60.0f), (float)g_framebufferSize.x / (float)g_framebufferSize.x, 0.1f),
        //.ProjectionMatrix = glm::perspectiveFovRH_ZO(glm::radians(60.0f), (float)g_framebufferSize.x, (float)g_framebufferSize.x, 0.1f, 1024.0f),
        .ViewMatrix = g_mainCamera.GetViewMatrix(),
        .CameraPosition = glm::vec4(g_mainCamera.Position, 0.0f)
    };

    auto globalUniformsBuffer = CreateBuffer("GlobalUniforms", sizeof(SGlobalUniforms), &globalUniforms, GL_DYNAMIC_DRAW);

    auto CreateWorldMatrix = [](float x, float y, float z) {
        return glm::translate(glm::mat4x4(1.0f), glm::vec3(x, y, z));
    };

    uint32_t mainAlbedoTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &mainAlbedoTexture);
    SetDebugLabel(mainAlbedoTexture, GL_TEXTURE, std::format("MainAlbedoTexture_{}x{}", g_framebufferSize.x, g_framebufferSize.y));
    glTextureStorage2D(mainAlbedoTexture, 1, GL_SRGB8_ALPHA8, g_framebufferSize.x, g_framebufferSize.y);

    uint32_t mainNormalTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &mainNormalTexture);
    SetDebugLabel(mainNormalTexture, GL_TEXTURE, std::format("MainNormalTexture_{}x{}", g_framebufferSize.x, g_framebufferSize.y));
    glTextureStorage2D(mainNormalTexture, 1, GL_RGBA32F, g_framebufferSize.x, g_framebufferSize.y);

    uint32_t mainDepthTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &mainDepthTexture);
    SetDebugLabel(mainDepthTexture, GL_TEXTURE, std::format("MainDepthTexture_{}x{}", g_framebufferSize.x, g_framebufferSize.y));
    glTextureStorage2D(mainDepthTexture, 1, GL_DEPTH32F_STENCIL8, g_framebufferSize.x, g_framebufferSize.y);

    uint32_t mainFramebuffer = 0;
    glCreateFramebuffers(1, &mainFramebuffer);
    SetDebugLabel(mainFramebuffer, GL_FRAMEBUFFER, "MainFramebuffer");    
    glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainAlbedoTexture, 0);
    glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT1, mainNormalTexture, 0);
    glNamedFramebufferTexture(mainFramebuffer, GL_DEPTH_ATTACHMENT, mainDepthTexture, 0);

    constexpr std::array<uint32_t, 8> drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_NONE };
    glNamedFramebufferDrawBuffers(mainFramebuffer, 8, drawBuffers.data());

    auto mainFramebufferStatus = glCheckNamedFramebufferStatus(mainFramebuffer, GL_FRAMEBUFFER);
    if (mainFramebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        auto errorMessage = std::string("Framebuffer incomplete");
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, errorMessage.size(), errorMessage.data());
        return -10;
    }

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

    uint32_t objectBuffer = 0;
    glCreateBuffers(1, &objectBuffer);
    glNamedBufferStorage(objectBuffer, sizeof(SObject) * 16384, nullptr, GL_DYNAMIC_STORAGE_BIT);

    uint32_t objectIndirectBuffer = 0;
    glCreateBuffers(1, &objectIndirectBuffer);
    glNamedBufferStorage(objectIndirectBuffer, sizeof(SDrawElementCommand) * 16384, nullptr, GL_DYNAMIC_STORAGE_BIT);

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
        "SM_Complex",
        "data/default/SM_Deccer_Cubes_Textured_Complex.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
        */
        /*
    AddModelFromFile(
        "SM_Beams",
        "data/default/SM_Beams_01.glb",
        megaVertexBuffer,
        megaIndexBuffer);
    */
        /*
    AddModelFromFile(
        "SM_Complex_Embedded",
        "data/default/SM_Deccer_Cubes_Textured_Embedded.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
        */
       
    AddModelFromFile(
        "SM_Complex",
        "data/default/SM_Deccer_Cubes_Textured.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
        
/*
    AddModelFromFile(
        "SM_Helmet",
        "data/scenes/DamagedHelmet/DamagedHelmet.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
*/
/*
    AddModelFromFile(
        "SM_Bistro",
        "data/scenes/Bistro52/scene.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
*/
/*
    AddModelFromFile(
        "SM_Tower",
        "data/scenes/Tower/scene.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
*/         
        
        /*
    AddModelFromFile(
        "Yargle",
        "data/default/mira_up/scene.gltf",
        megaVertexBuffer,
        megaIndexBuffer);
        */

    auto& meshNames = g_modelToPrimitiveMap["SM_Complex"];

    // prepare material buffer, instance and indirect draw buffer

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

    for (auto meshIndex = 0; auto& meshName : meshNames) {

        auto& mesh = g_primitiveToMeshMap[meshName];
        auto& transform = g_primitiveToInitialTransformMap[meshName];

        SObject object = {
            .WorldMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)) * transform,
            .InstanceParameter = glm::ivec4(mesh.MaterialIndex, 0, 0, 0)
        };
        glNamedBufferSubData(objectBuffer, sizeof(SObject) * meshIndex, sizeof(SObject), &object);

        SDrawElementCommand drawElementCommand = {
            .IndexCount = static_cast<uint32_t>(mesh.IndexCount),
            .InstanceCount = 1,
            .FirstIndex = static_cast<uint32_t>(mesh.IndexOffset),
            .BaseVertex = static_cast<int32_t>(mesh.VertexOffset),
            .BaseInstance = 0
        };
        glNamedBufferSubData(objectIndirectBuffer, sizeof(SDrawElementCommand) * meshIndex, sizeof(SDrawElementCommand), &drawElementCommand);

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

        ZoneScopedN("Render");
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
        glNamedBufferSubData(debugOptionsBuffer, 0, sizeof(SDebugOptions), &g_debugOptions);

        auto framebufferWasResized = false;


        if (g_sceneViewerResized || g_framebufferResized) {

            ZoneScopedN("Resize");
            //glm::vec2 scaledFramebufferSize = glm::vec2{0.0f, 0.0f};
            if (g_isEditor) {
                scaledFramebufferSize = glm::vec2(g_sceneViewerSize) * windowSettings.ResolutionScale;
                g_sceneViewerResized = false;
            } else {
                scaledFramebufferSize = glm::vec2(g_framebufferSize) * windowSettings.ResolutionScale;
                g_framebufferResized = false;
            }

            glDeleteTextures(1, &mainAlbedoTexture);
            glCreateTextures(GL_TEXTURE_2D, 1, &mainAlbedoTexture);
            SetDebugLabel(mainAlbedoTexture, GL_TEXTURE, std::format("MainTexture_{}x{}", scaledFramebufferSize.x, scaledFramebufferSize.y));
            glTextureStorage2D(mainAlbedoTexture, 1, GL_RGBA8, scaledFramebufferSize.x, scaledFramebufferSize.y);

            glDeleteTextures(1, &mainNormalTexture);
            glCreateTextures(GL_TEXTURE_2D, 1, &mainNormalTexture);
            SetDebugLabel(mainNormalTexture, GL_TEXTURE, std::format("MainNormalTexture_{}x{}", scaledFramebufferSize.x, scaledFramebufferSize.y));
            glTextureStorage2D(mainNormalTexture, 1, GL_RGBA32F, scaledFramebufferSize.x, scaledFramebufferSize.y);  

            glDeleteTextures(1, &mainDepthTexture);
            glCreateTextures(GL_TEXTURE_2D, 1, &mainDepthTexture);
            SetDebugLabel(mainDepthTexture, GL_TEXTURE, std::format("MainDepthTexture_{}x{}", scaledFramebufferSize.x, scaledFramebufferSize.y));
            glTextureStorage2D(mainDepthTexture, 1, GL_DEPTH32F_STENCIL8, scaledFramebufferSize.x, scaledFramebufferSize.y);

            glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT0, mainAlbedoTexture, 0);
            glNamedFramebufferTexture(mainFramebuffer, GL_COLOR_ATTACHMENT1, mainNormalTexture, 0);
            glNamedFramebufferTexture(mainFramebuffer, GL_DEPTH_ATTACHMENT, mainDepthTexture, 0);            

            constexpr std::array<uint32_t, 8> drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_NONE };
            glNamedFramebufferDrawBuffers(mainFramebuffer, 8, drawBuffers.data());
            glNamedFramebufferTexture(mainFramebuffer, GL_DEPTH_ATTACHMENT, mainDepthTexture, 0);

            framebufferWasResized = true;
        }

        glViewport(0, 0, scaledFramebufferSize.x, scaledFramebufferSize.y);

        if (isSrgbDisabled) {
            glEnable(GL_FRAMEBUFFER_SRGB);
            isSrgbDisabled = false;
        }

        // Depth Pre Pass


        // GBuffer Pass

        PushDebugGroup("SimplePipeline");

        if (g_debugShowMaterialId) {
            glBindProgramPipeline(simpleDebugProgramPipeline);
        } else {
            glBindProgramPipeline(simpleProgramPipeline);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);

        auto clearDepth = 1.0f;
        glDisable(GL_FRAMEBUFFER_SRGB);
        glColorMaski(0, true, true, true, true);
        glColorMaski(1, true, true, true, true);
        glClearNamedFramebufferfv(mainFramebuffer, GL_COLOR, 0, glm::value_ptr(glm::vec4{0.4f, 0.1f, 0.6f, 1.0f}));
        glClearNamedFramebufferfv(mainFramebuffer, GL_COLOR, 1, glm::value_ptr(glm::vec4{0.0f, 0.0f, 0.0f, 0.0f}));
        glClearNamedFramebufferfv(mainFramebuffer, GL_DEPTH, 0, &clearDepth);
        glEnable(GL_FRAMEBUFFER_SRGB);

        glVertexArrayElementBuffer(g_defaultInputLayout, megaIndexBuffer);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUniformsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, megaVertexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, objectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, gpuMaterialBuffer);

        if (g_debugShowMaterialId) {
            //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 19, cpuMaterialBuffer);
            //glBindBufferBase(GL_UNIFORM_BUFFER, 20, debugOptionsBuffer);
        }

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
        ImGui::SetNextWindowSize({256, 192});
        if (ImGui::Begin("Huhu", &isOpen, ImGuiWindowFlags_::ImGuiWindowFlags_Modal | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration)) {

            ImGui::TextColored(ImVec4{0.6f, 0.1f, 0.8f, 1.0f}, "Press F1 to toggle editor");
            ImGui::Separator();

            auto framesPerSecond = 1.0f / deltaTimeInSeconds;
            ImGui::Text("afps: %.0f rad/s", glm::two_pi<float>() * framesPerSecond);
            ImGui::Text("dfps: %.0f °/s", glm::degrees(glm::two_pi<float>() * framesPerSecond));
            ImGui::Text("rfps: %.0f", framesPerSecond);
            ImGui::Text("rpms: %.0f", framesPerSecond * 60.0f);
            ImGui::Text("  ft: %.2f ms", deltaTimeInSeconds * 1000.0f);
#if USE_LILLYPAD
            ImGui::Separator();
            ImGui::Text("temp: %d °C", gpuInformation.GpuCoreTemperature);
            ImGui::Text("  cs: %d (%d)/(%d) MHz", gpuInformation.ClockSpeedCurrent, gpuInformation.ClockSpeedMin, gpuInformation.ClockSpeedMax);
            ImGui::Text(" mcs: %d (%d)/(%d) MHz", gpuInformation.MemoryClockSpeed, gpuInformation.MemoryClockSpeedMin, gpuInformation.MemoryClockSpeedMax);
#endif
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
                    ? mainAlbedoTexture
                    : mainNormalTexture;
                ImGui::Image(reinterpret_cast<ImTextureID>(texture), availableSceneWindowSize, g_imvec2UnitY, g_imvec2UnitX);
            }
            ImGui::PopStyleVar();
            ImGui::End();
        } else {

            PushDebugGroup("Blit To UI");
            glViewport(0, 0, g_framebufferSize.x, g_framebufferSize.y);
            DrawFullscreenTriangle(mainAlbedoTexture);
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

        frameCounter++;
        if ((frameCounter % 2000) == 0) {
#if USE_LILLYPAD            
            UpdateGpuInformation(0, &gpuInformation);
#endif
        }
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
    glDeleteTextures(1, &mainAlbedoTexture);
    glDeleteTextures(1, &mainDepthTexture);
    glDeleteTextures(1, &mainNormalTexture);

    glDeleteFramebuffers(1, &mainFramebuffer);

    glDeleteBuffers(1, &objectBuffer);

    glDeleteVertexArrays(1, &g_defaultInputLayout);

    glDeleteProgram(simpleVertexShader);
    glDeleteProgram(simpleFragmentShader);
    glDeleteProgram(simpleDebugFragmentShader);
    glDeleteProgram(fullscreenTriangleVertexShader);
    glDeleteProgram(fullscreenTriangleFragmentShader);
    glDeleteProgramPipelines(1, &simpleDebugProgramPipeline);
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
