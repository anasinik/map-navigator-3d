#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform float uEmissive;

// Red lights from spheres
uniform int uNumRedLights;
uniform vec3 uRedLightPositions[10];

void main() {
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;

    // Main light (white from above)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;

    // Red lights contribution
    vec3 redLightContribution = vec3(0.0);
    for (int i = 0; i < uNumRedLights; i++) {
        vec3 redLightDir = normalize(uRedLightPositions[i] - FragPos);
        float redDistance = length(uRedLightPositions[i] - FragPos);
        
        float attenuation = 1.0 / (1.0 + 0.5 * redDistance + 0.3 * redDistance * redDistance);
        
        // Diffuse from red light
        float redDiff = max(dot(norm, redLightDir), 0.0);
        vec3 redLightColor = vec3(1.0, 0.0, 0.0); // Red light
        
        redLightContribution += redDiff * redLightColor * attenuation;
    }

    // Emissive component (self-illumination)
    vec3 emissive = uEmissive * uColor;

    // Final color
    vec3 result = (ambient + diffuse + specular) * uColor + redLightContribution + emissive;
    FragColor = vec4(result, 1.0);
}