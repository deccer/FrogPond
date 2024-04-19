#include "Framebuffer.hpp"
#include "DebugLabel.hpp"

#include <cstdint>
#include <format>
#include <string>

#include <glm/gtc/type_ptr.hpp>

#include <glad/gl.h>

auto FormatToAttachmentType(EFormat attachmentFormat, size_t colorAttachmentIndex) -> EAttachmentType {

    switch (attachmentFormat) {
        case EFormat::D24S8_Float:
        case EFormat::D32_Float:
            return EAttachmentType::DepthAttachment;
        default:
            return static_cast<EAttachmentType>(static_cast<std::underlying_type<EAttachmentType>::type>(EAttachmentType::ColorAttachment0) + colorAttachmentIndex);
    }
}

auto AttachmentTypeToAttachmentKind(EAttachmentType attachment) -> EAttachmentKind {

    return attachment == EAttachmentType::DepthAttachment
        ? EAttachmentKind::Depth
        : EAttachmentKind::Color;
}

auto ToGL(EAttachmentType attachmentType) -> uint32_t {
    switch (attachmentType) {
        case EAttachmentType::ColorAttachment0: return GL_COLOR_ATTACHMENT0;
        case EAttachmentType::ColorAttachment1: return GL_COLOR_ATTACHMENT1;
        case EAttachmentType::ColorAttachment2: return GL_COLOR_ATTACHMENT2;
        case EAttachmentType::ColorAttachment3: return GL_COLOR_ATTACHMENT3;
        case EAttachmentType::ColorAttachment4: return GL_COLOR_ATTACHMENT4;
        case EAttachmentType::ColorAttachment5: return GL_COLOR_ATTACHMENT5;
        case EAttachmentType::ColorAttachment6: return GL_COLOR_ATTACHMENT6;
        case EAttachmentType::ColorAttachment7: return GL_COLOR_ATTACHMENT7;
        case EAttachmentType::DepthAttachment: return GL_DEPTH_ATTACHMENT;
        case EAttachmentType::StencilAttachment: return GL_STENCIL_ATTACHMENT;
        default:
            std::string message = "AttachmentType not mappable";
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, message.size(), message.data());
            return -1;        
    }
}

auto ToGL(EAttachmentKind attachmentKind) -> uint32_t {
    switch (attachmentKind) {
        case EAttachmentKind::Color: return GL_COLOR;
        case EAttachmentKind::Depth: return GL_DEPTH;
        case EAttachmentKind::Stencil: return GL_STENCIL;
        default:
            std::string message = "AttachmentKind not mappable";
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, message.size(), message.data());
            return -1;    
    }
}

auto CreateFramebuffer(
    std::string_view label,
    uint32_t width,
    uint32_t height,
    std::span<SFramebufferAttachmentDescriptor> attachmentsDescriptors) -> SFramebuffer {

    std::array<uint32_t, 8> drawBuffers = {};
    std::fill_n(drawBuffers.begin(), 8, GL_NONE);

    SFramebuffer framebuffer = {};
    framebuffer.Width = width;
    framebuffer.Height = height;

    glCreateFramebuffers(1, &framebuffer.Id);
    SetDebugLabel(framebuffer.Id, GL_FRAMEBUFFER, label);
    framebuffer.Label = label;

    for (auto attachmentIndex = 0; auto attachmentDescriptor : attachmentsDescriptors) {

        SFramebufferAttachment attachment = {};
        attachment.Format = attachmentDescriptor.Format;
        attachment.ClearValue = std::move(attachmentDescriptor.ClearValue);
        attachment.Type = FormatToAttachmentType(attachment.Format, attachmentIndex);
        attachment.Kind = AttachmentTypeToAttachmentKind(attachment.Type);

        uint32_t attachmentId = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &attachmentId);
        SetDebugLabel(attachmentId, GL_TEXTURE, std::format("{}_{}x{}", label, width, height));
        glTextureStorage2D(attachmentId, 1, ToGL(attachment.Format), width, height);

        glNamedFramebufferTexture(framebuffer.Id, ToGL(attachment.Type), attachmentId, 0);

        if (attachment.Type != EAttachmentType::DepthAttachment && attachment.Type != EAttachmentType::StencilAttachment) {
            drawBuffers[attachmentIndex] = ToGL(attachment.Type);
        }

        attachment.AttachmentId = attachmentId;
        framebuffer.Attachments.push_back(std::move(attachment));

        attachmentIndex++;
    }

    glNamedFramebufferDrawBuffers(framebuffer.Id, 8, drawBuffers.data());

    auto framebufferStatus = glCheckNamedFramebufferStatus(framebuffer.Id, GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        auto message = std::format("Framebuffer {} is incomplete", framebuffer.Label);
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 1, GL_DEBUG_SEVERITY_HIGH, message.size(), message.data());
    }

    return framebuffer;
}

auto ResizeFramebuffer(
    SFramebuffer& framebuffer,
    uint32_t width,
    uint32_t height) -> void {

    std::array<uint32_t, 8> drawBuffers = {};
    std::fill_n(drawBuffers.begin(), 8, GL_NONE);

    for (auto attachmentIndex = 0; auto& attachment : framebuffer.Attachments) {

        uint32_t attachmentId = attachment.AttachmentId;
        glDeleteTextures(1, &attachmentId);

        glCreateTextures(GL_TEXTURE_2D, 1, &attachmentId);
        SetDebugLabel(attachmentId, GL_TEXTURE, std::format("{}_{}x{}", framebuffer.Label, width, height));
        glTextureStorage2D(attachmentId, 1, ToGL(attachment.Format), width, height);

        glNamedFramebufferTexture(framebuffer.Id, ToGL(attachment.Type), attachmentId, 0);

        if (attachment.Type != EAttachmentType::DepthAttachment && attachment.Type != EAttachmentType::StencilAttachment) {
            drawBuffers[attachmentIndex] = ToGL(attachment.Type);
        }

        attachment.AttachmentId = attachmentId;

        attachmentIndex++;
    }

    glNamedFramebufferDrawBuffers(framebuffer.Id, 8, drawBuffers.data());

    auto framebufferStatus = glCheckNamedFramebufferStatus(framebuffer.Id, GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        auto message = std::format("Framebuffer {} is incomplete", framebuffer.Label);
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 1, GL_DEBUG_SEVERITY_HIGH, message.size(), message.data());
    }    

    framebuffer.Width = width;
    framebuffer.Height = height;
}

auto DestroyFramebuffer(SFramebuffer& framebuffer) -> void {

    for (auto attachment : framebuffer.Attachments) {
        glDeleteTextures(1, &attachment.AttachmentId);
    }
    glDeleteFramebuffers(1, &framebuffer.Id);

    framebuffer.Attachments.clear();
}

auto BindFramebuffer(const SFramebuffer& framebuffer) -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.Id);
}

auto ClearFramebuffer(const SFramebuffer& framebuffer) -> void {
    for (auto attachmentIndex = 0; auto attachment : framebuffer.Attachments) {
        if (attachment.ClearValue.has_value()) {
            glClearNamedFramebufferfv(
                framebuffer.Id,
                ToGL(attachment.Kind),
                attachment.Kind == EAttachmentKind::Depth ? 0 : attachmentIndex, 
                attachment.Kind == EAttachmentKind::Depth ? &(*attachment.ClearValue).r : glm::value_ptr(*attachment.ClearValue));
        }

        attachmentIndex++;
    }
}