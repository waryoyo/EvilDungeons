#version 460 core

in vec2 vUV;
in vec3 vNormal;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, vUV);
    FragColor = texColor;
}
