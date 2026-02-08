#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec4 textColor;        
uniform bool useTexture;

void main() {
    if (useTexture) {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
        FragColor = vec4(textColor.rgb, 1.0) * sampled;
    } else {
        FragColor = textColor;
    }
}
