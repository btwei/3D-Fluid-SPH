#version 450 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inProperties;

layout (location = 0) out vec2 outProperties;

void main() {
    outProperties = inProperties;

    gl_Position = vec4(inPosition, 0.0, 1.0);
}