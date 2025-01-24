#version 450 core

in vec4 properties;

out vec4 outColor;

void main() {
    outColor = vec4(properties.y / 10000.0, 0.5f, 0.8f, 1.0f);
}