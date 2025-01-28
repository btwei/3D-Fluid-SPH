#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

void main() {
    vec3 color = texture(colorTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}