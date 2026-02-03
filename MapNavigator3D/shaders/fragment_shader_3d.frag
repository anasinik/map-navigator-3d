#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

void main() {
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // Combine
    vec3 result = (ambient + diffuse) * texture(uTexture, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}