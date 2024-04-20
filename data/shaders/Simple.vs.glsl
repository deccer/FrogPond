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
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    //mat4 view_projection_matrix;
    vec4 CameraPosition;
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

struct SVertexPosition
{
    SPackedVec3 Position;
};

struct SVertexNormalUv
{
    uint Normal;
    SPackedVec2 Uv;
};

layout(binding = 1, std430) restrict readonly buffer VertexPositionBuffer
{
    SVertexPosition VertexPositions[];
};

layout(binding = 2, std430) restrict readonly buffer VertexNormalUvBuffer
{
    SVertexNormalUv VertexNormalUvs[];
};

struct SObject
{
    mat4 WorldMatrix;
    ivec4 InstanceParameter;
};

layout (binding = 3, std430) restrict readonly buffer ObjectsBuffer
{
    SObject Objects[];
};

vec2 SignNotZero(vec2 v)
{
    return vec2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

vec3 DecodeNormal(vec2 encodedNormal)
{
    vec3 decodedNormal = vec3(encodedNormal.xy, 1.0 - abs(encodedNormal.x) - abs(encodedNormal.y));
    if (decodedNormal.z < 0) {
        decodedNormal.xy = (1.0 - abs(decodedNormal.yx)) * SignNotZero(decodedNormal.xy);
    }

    return normalize(decodedNormal);
}

void main()
{
    SVertexPosition vertex_position = VertexPositions[gl_VertexID];
    SVertexNormalUv vertex_normal_uv = VertexNormalUvs[gl_VertexID];
    SObject object = Objects[gl_DrawID];

    v_normal = DecodeNormal(unpackSnorm2x16(vertex_normal_uv.Normal));
    v_uv = PackedToVec2(vertex_normal_uv.Uv);
    v_material_id = object.InstanceParameter.x;

    gl_Position = u_camera_information.ProjectionMatrix *
                  u_camera_information.ViewMatrix *
                  object.WorldMatrix * 
                  vec4(PackedToVec3(vertex_position.Position), 1.0);
}