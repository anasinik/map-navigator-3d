#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D iconTexture;

void main() {
    vec4 texColor = texture(iconTexture, TexCoords);
    color = texColor;
}