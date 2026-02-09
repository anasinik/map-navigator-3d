#version 330 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

uniform int uNumRedLights;
uniform vec3 uRedLightPositions[100];

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    float specularStrength = 0.2;
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * uLightColor;

    vec3 redLightContribution = vec3(0.0);
    for (int i = 0; i < uNumRedLights; i++) {
        vec3 redLightDir = normalize(uRedLightPositions[i] - FragPos);
        float redDistance = length(uRedLightPositions[i] - FragPos);
        
        float attenuation = 1.0 / (1.0 + 0.3 * redDistance + 0.1 * redDistance * redDistance);
        
        float redDiff = max(dot(norm, redLightDir), 0.0);
        vec3 redLightColor = vec3(1.0, 0.0, 0.0);
        
        float intensity = 2.0;
        
        redLightContribution += redDiff * redLightColor * attenuation * intensity;
    }

    vec3 result = (ambient + diffuse + specular + redLightContribution) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}