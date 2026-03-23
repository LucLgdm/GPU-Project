#version 430 core

in vec3 vNormal;
in vec3 vFragPos;

uniform vec3 uLightDir;
uniform vec3 uColor;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(vNormal);
    float diff = max(dot(norm, normalize(uLightDir)), 0.0);
    vec3 ambient = 0.2 * uColor;
    vec3 diffuse = diff * uColor;
    fragColor = vec4(ambient + diffuse, 1.0);
}