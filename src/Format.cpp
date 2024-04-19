#include "Format.hpp"

#include <cstdint>
#include <string>

#include <glad/gl.h>

auto ToGL(EFormat format) -> uint32_t {
    switch (format) {
        case EFormat::D24S8_Float: return GL_DEPTH24_STENCIL8;
        case EFormat::D32_Float: return GL_DEPTH32F_STENCIL8;
        case EFormat::R32G32B32A32_Float: return GL_RGBA32F;
        case EFormat::R8G8B8A8_Srgb: return GL_SRGB8_ALPHA8;
        default:
            std::string message = "Format not mappable";
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, message.size(), message.data());
            return -1;
    }
}