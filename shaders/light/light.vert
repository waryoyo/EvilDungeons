#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 ObjectColor;

uniform mat4 uMVP;
uniform vec3 uObjectColor;

void main()
{
    ObjectColor = uObjectColor;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
