#version 460 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec4 i_color;

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

struct object_t
{
    mat4 world_matrix;
};

layout (binding = 1) buffer Objects
{
    object_t Objects[];
} u_objects;

void main()
{
    object_t object = u_objects.Objects[gl_InstanceID];

    v_color = i_color;
    gl_Position = u_camera_information.projection_matrix *
                  u_camera_information.view_matrix *
                  object.world_matrix * 
                  vec4(i_position, 1.0);
}