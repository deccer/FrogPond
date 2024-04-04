#version 460 core

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};
layout (location = 0) out vec3 v_normal;
layout (location = 1) out vec2 v_uv;
layout (location = 2) flat out uint v_material_id;

layout (location = 0, std140) uniform CameraInformation
{
    //mat4 old_projection_matrix;
    //mat4 old_view_matrix;
    //mat4 old_view_projection_matrix;
    mat4 projection_matrix;
    mat4 view_matrix;
    //mat4 view_projection_matrix;
    vec4 camera_position;
    //vec4 frustum_planes[6];
    //vec4 viewport;
} u_camera_information;

struct SPackedVec2
{
    float x;
    float y;
};

struct SPackedVec3
{
    float x;
    float y;
    float z;
};

struct SPackedVec4
{
    float x;
    float y;
    float z;
    float w;
};

vec2 PackedToVec2(in SPackedVec2 v)
{
    return vec2(v.x, v.y);
}

SPackedVec2 Vec2ToPacked(in vec2 v)
{
    return SPackedVec2(v.x, v.y);
}

vec3 PackedToVec3(in SPackedVec3 v)
{
    return vec3(v.x, v.y, v.z);
}

SPackedVec3 Vec3ToPacked(in vec3 v)
{
    return SPackedVec3(v.x, v.y, v.z);
}

vec4 PackedToVec4(in SPackedVec4 v)
{
    return vec4(v.x, v.y, v.z, v.w);
}

SPackedVec4 Vec4ToPacked(in vec4 v)
{
    return SPackedVec4(v.x, v.y, v.z, v.w);
}

struct SVertexPositionNormalUv
{
    SPackedVec3 position;
    SPackedVec3 normal;
    SPackedVec2 uv;
};

layout(binding = 1, std430) restrict readonly buffer VertexBuffer
{
    SVertexPositionNormalUv Vertices[];
};

struct SObject
{
    mat4 world_matrix;
    ivec4 instance_parameter;
};

layout (binding = 2, std430) restrict readonly buffer ObjectsBuffer
{
    SObject Objects[];
};

void main()
{
    SVertexPositionNormalUv vertex = Vertices[gl_VertexID];
    SObject object = Objects[gl_DrawID];

    v_normal = PackedToVec3(vertex.normal);
    v_uv = PackedToVec2(vertex.uv);
    v_material_id = object.instance_parameter.x;

    gl_Position = u_camera_information.projection_matrix *
                  u_camera_information.view_matrix *
                  object.world_matrix * 
                  vec4(PackedToVec3(vertex.position), 1.0);
}