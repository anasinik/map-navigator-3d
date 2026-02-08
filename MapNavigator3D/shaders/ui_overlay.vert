#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 uProjection;
uniform bool uIgnoreTransform;

void main() {
    if (uIgnoreTransform) {
        gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    } else {
        gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    }
    TexCoord = aTexCoord;
}
