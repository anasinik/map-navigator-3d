#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D uDiffMap1;
uniform sampler2D uSpecMap1;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

void main() {

    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;

    vec3 texColor = texture(uDiffMap1, TexCoords).rgb;
    if (texColor == vec3(0.0)) texColor = vec3(0.8, 0.6, 0.4);

    vec3 result = (ambient + diffuse + specular) * texColor;
    
    FragColor = vec4(result, 1.0);
}