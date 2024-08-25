#version 330

in vec2 uv_coord;

uniform sampler2D u_texture;

out vec4 frag_color;

void main() {
    frag_color = texture(u_texture, uv_coord);
}
