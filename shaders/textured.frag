#version 330

in vec2 tex_coords;

uniform sampler2D text;

out vec4 frag_color;

void main() {
    frag_color = texture(text, tex_coords);
}
