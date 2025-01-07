#version 450 core

layout (location = 0) in vec2 inProperties;

out vec4 outColor;

void main() {
    outColor = vec4(inProperties, 0.0f, 1.0f);
}