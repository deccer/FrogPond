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
    vec4 base_color;

    uint64_t base_texture_handle;
    uint64_t normal_texture_handle;
    uint64_t occlusion_texture_handle;
    uint64_t metallic_roughness_texture_handle;

    uint64_t emissive_texture_handle;
    uint64_t _padding1;
};

layout (binding = 4, std430) readonly buffer GpuMaterialBuffer
{
    SGpuMaterial GpuMaterials[];
};

layout (binding = 5, std140) uniform ShadingBuffer
{
    vec4 SunDirection;
    vec4 SunStrength;
};

void main()
{
    SGpuMaterial material = GpuMaterials[v_material_id];
    
    float sun_n_dot_l = clamp(dot(v_normal, -SunDirection.xyz), 0.0, 1.0);

    o_color = texture(sampler2D(material.base_texture_handle), v_uv) * vec4(SunStrength.rgb * sun_n_dot_l, 1.0);
    o_normal = vec4(v_normal * 0.5 + 0.5, 1.0);
}