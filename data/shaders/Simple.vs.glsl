#version 460 core

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};
layout (location = 0) out vec4 v_color;

layout (location = 0) uniform CameraInformation
{
    mat4 projection_matrix;
    mat4 view_matrix;
} u_camera_information;

struct vec2_packed_t
{
    float x;
    float y;
};

struct vec3_packed_t
{
    float x;
    float y;
    float z;
};

struct vec4_packed_t
{
    float x;
    float y;
    float z;
    float w;
};

vec2 PackedToVec2(in vec2_packed_t v)
{
    return vec2(v.x, v.y);
}

vec2_packed_t Vec2ToPacked(in vec2 v)
{
    return vec2_packed_t(v.x, v.y);
}

vec3 PackedToVec3(in vec3_packed_t v)
{
    return vec3(v.x, v.y, v.z);
}

vec3_packed_t Vec3ToPacked(in vec3 v)
{
    return vec3_packed_t(v.x, v.y, v.z);
}

vec4 PackedToVec4(in vec4_packed_t v)
{
    return vec4(v.x, v.y, v.z, v.w);
}

vec4_packed_t Vec4ToPacked(in vec4 v)
{
    return vec4_packed_t(v.x, v.y, v.z, v.w);
}

struct vertex_t
{
    vec3_packed_t position;
    vec4_packed_t color;
};

layout(std430, binding = 1) restrict readonly buffer VertexBuffer
{
    vertex_t Vertices[];
};

struct object_t
{
    mat4 world_matrix;
};

layout (binding = 2) buffer ObjectsBuffer
{
    object_t Objects[];
};

void main()
{
    vertex_t vertex = Vertices[gl_VertexID];
    object_t object = Objects[gl_InstanceID];

    v_color = PackedToVec4(vertex.color);
    gl_Position = u_camera_information.projection_matrix *
                  u_camera_information.view_matrix *
                  object.world_matrix * 
                  vec4(PackedToVec3(vertex.position), 1.0);
}