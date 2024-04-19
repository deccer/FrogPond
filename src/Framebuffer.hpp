#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <glm/vec4.hpp>

#include "Format.hpp"

enum class EAttachmentType : uint32_t {
    ColorAttachment0 = 0u,
    ColorAttachment1,
    ColorAttachment2,
    ColorAttachment3,
    ColorAttachment4,
    ColorAttachment5,
    ColorAttachment6,
    ColorAttachment7,
    DepthAttachment,
    StencilAttachment
};

enum class EAttachmentKind {
    Color,
    Depth,
    Stencil
};

struct SFramebufferAttachmentDescriptor {
    EFormat Format; // GL_SRGB8_ALPHA8, GL_RGB32F, ...
    std::optional<glm::vec4> ClearValue;
};

struct SFramebufferAttachment {
    uint32_t AttachmentId;    
    EFormat Format; // GL_SRGB8_ALPHA8, GL_RGB32F, ...
    EAttachmentType Type; // GL_COLOR_ATTACHMENT0, ... GL_DEPTH_ATTACHMENT
    EAttachmentKind Kind; // GL_COLOR, GL_DEPTH
    std::optional<glm::vec4> ClearValue;
};

struct SFramebuffer {
    std::vector<SFramebufferAttachment> Attachments;
    uint32_t Width;
    uint32_t Height;    
    uint32_t Id;
    std::string_view Label;
};

auto CreateFramebuffer(
    std::string_view label,
    uint32_t width,
    uint32_t height,
    std::span<SFramebufferAttachmentDescriptor> attachmentsDescriptors) -> SFramebuffer;
auto ResizeFramebuffer(
    SFramebuffer& framebuffer,
    uint32_t width,
    uint32_t height) -> void;
auto DestroyFramebuffer(SFramebuffer& framebuffer) -> void;
auto BindFramebuffer(const SFramebuffer& framebuffer) -> void;
auto ClearFramebuffer(const SFramebuffer& framebuffer) -> void;