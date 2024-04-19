#pragma once

#include <cstdint>

enum class EFormat {
    R8G8B8A8_Srgb,
    R32G32B32A32_Float,
    D24S8_Float,
    D32_Float
};

auto ToGL(EFormat format) -> uint32_t;