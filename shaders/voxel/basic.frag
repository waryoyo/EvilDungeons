#version 460 core

in vec2 vUV;
in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uCameraPos;
uniform vec3 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;

void main() {
    vec4 texColor = texture(uTexture, vUV); // A contains texture alpha

    float dist = distance(uCameraPos, vWorldPos);
    float fogFactor = clamp((uFogEnd - dist) / (uFogEnd - uFogStart), 0.0, 1.0);

    vec4 finalColor = mix(vec4(uFogColor, texColor.a), texColor, fogFactor);
    FragColor = finalColor;
}
