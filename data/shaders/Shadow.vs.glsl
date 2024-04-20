#version 460 core

layout (location = 0) in vec3 i_position;

layout (location = 0) uniform int u_global_light_index;

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

struct SGpuGlobalLight
{
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
};

layout (binding = 2, std430) readonly buffer GpuGlobalLights
{
    SGpuGlobalLight Lights[];
} globalLights;

struct SObject
{
    mat4 WorldMatrix;
    ivec4 InstanceParameter;
};

layout (binding = 3, std430) restrict readonly buffer ObjectsBuffer
{
    SObject Objects[];
};

void main()
{
    SGpuGlobalLight global_light = globalLights.Lights[u_global_light_index];
    gl_Position = global_light.ProjectionMatrix *
                  global_light.ViewMatrix *
                  instanceBuffer.Instances[gl_DrawID].WorldMatrix * vec4(i_position, 1.0);
}