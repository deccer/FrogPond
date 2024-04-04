#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_NV_gpu_shader5 : require
#extension GL_ARB_gpu_shader_int64 : require

layout (location = 0) in vec3 v_normal;
layout (location = 1) in vec2 v_uv;
layout (location = 2) flat in uint v_material_id;

layout (location = 0) out vec4 o_color;
layout (location = 1) out vec4 o_normal;

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

void main()
{
    SGpuMaterial material = GpuMaterials[v_material_id];
    o_color = texture(sampler2D(material.base_texture), v_uv);
    o_normal = vec4(0.5 * texture(sampler2D(material.normal_texture), v_uv).rgb + 0.5 * (v_normal * 0.5 + 0.5), 1.0);
}