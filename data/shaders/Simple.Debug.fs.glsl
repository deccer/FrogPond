#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_NV_gpu_shader5 : require
#extension GL_ARB_gpu_shader_int64 : require

layout (location = 0) in vec3 v_normal;
layout (location = 1) in vec2 v_uv;
layout (location = 2) flat in uint v_material_id;

layout (location = 0) out vec4 o_color;
layout (location = 1) out vec4 o_normal;

struct SCpuMaterial
{
    uint32_t BaseTextureIndex;
    uint32_t NormalTextureIndex;
    uint32_t OcclusionTextureIndex;
    uint32_t MetallicRoughnessTextureIndex;
    uint32_t EmissiveTextureIndex;

    uint32_t _padding1;
    uint32_t _padding2;
    uint32_t _padding3;

    vec4 BaseColor;
};

struct SGpuMaterial
{
    uint64_t base_texture;
    uint64_t normal_texture;
    uint64_t occlusion_texture;
    uint64_t metallic_roughness_texture;

    uint64_t emissive_texture;
    uvec2 _padding1;

    vec4 base_color;
};

layout (binding = 3, std430) readonly buffer GpuMaterialBuffer
{
    SGpuMaterial GpuMaterials[];
};

layout (binding = 19, std430) readonly buffer CpuMaterialBuffer
{
    SCpuMaterial CpuMaterials[];
};

layout (location = 20, std140) uniform DebugOptions
{
    int show_material_id;
} u_debug_options;

vec3 HsvToRgb(in vec3 hsv)
{
    vec3 rgb = clamp(abs(mod(hsv.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
    return hsv.z * mix(vec3(1.0), rgb, hsv.y);
}

void main()
{
    const float GOLDEN_CONJ = 0.6180339887498948482045868343656;

    SGpuMaterial material = GpuMaterials[v_material_id];
    o_color = vec4(2.0 * HsvToRgb(vec3(v_material_id * 2 * GOLDEN_CONJ, 0.675, 0.65)), 1.0);
    o_normal = vec4(0.5 *texture(sampler2D(material.normal_texture), v_uv).rgb + 0.5 * (v_normal * 0.5 + 0.5), 1.0);
}