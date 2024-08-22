#version 330

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coords;

uniform mat4 u_matrix;

out vec2 tex_coords;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 1.0);
    tex_coords = a_tex_coords;
}
