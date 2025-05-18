#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;   
layout(location = 2) in vec3 aNormal;

out vec2 vUV;
out vec3 vNormal;

uniform mat4 uMVP;
uniform mat4 uModel;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vUV = aUV; 
}
