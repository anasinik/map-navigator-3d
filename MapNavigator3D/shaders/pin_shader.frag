#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform float uEmissive;

// Point lights from red spheres
uniform int uNumRedLights;
uniform vec3 uRedLightPositions[10];

void main() {
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * uLightColor;
    
    // Main light (sun/directional)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // Specular from main light
    float specularStrength = 0.3;
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;
    
    // Add red point lights
    vec3 redLighting = vec3(0.0);
    for (int i = 0; i < uNumRedLights; i++) {
        vec3 redLightDir = normalize(uRedLightPositions[i] - FragPos);
        float distance = length(uRedLightPositions[i] - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // Red diffuse light
        float redDiff = max(dot(norm, redLightDir), 0.0);
        vec3 redColor = vec3(1.0, 0.0, 0.0); // Pure red
        float intensity = 3.0; // Bright intensity
        
        redLighting += redDiff * redColor * intensity * attenuation;
    }
    
    // Emissive (for the red sphere itself)
    vec3 emissive = uEmissive * uColor;
    
    // Combine all lighting
    vec3 result = (ambient + diffuse + specular + redLighting) * uColor + emissive;
    FragColor = vec4(result, 1.0);
}
