#version 460 core

layout (location = 0) in vec3 v_normal;
layout (location = 1) in vec2 v_uv;

layout (location = 0) out vec4 o_color;

void main()
{
    o_color = vec4(v_normal + (vec3(v_uv, 0.0) * 0.5), 1.0);
}