#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 2) in vec4 inProperties;

uniform mat4 viewMatrix;

out vec4 properties;

mat4 m = mat4(1.0, 0.0, 0.0, 0.0,
              0.0, 1.0, 0.0, 0.0,
              0.0, 0.0, 1.0, 0.0,
              0.0, 0.0, -1.5, 1.0);

mat4 p = mat4(0.5, 0.0, 0.0, 0.0,
              0.0, 0.5, 0.0, 0.0,
              0.0, 0.0, -0.5, -1.0,
              0.0, 0.0, 0.0, 1.0);

void main() {
    properties = inProperties;

    gl_Position = p * viewMatrix * m * vec4(inPosition, 1.0);
    gl_PointSize = 5.0;
}