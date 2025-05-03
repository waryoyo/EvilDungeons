#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec2 aUV;

out vec2 oUV;

void main()
{
    oUV = aUV;
    gl_Position = vec4(aPos, 1.0);

}
