#version 460 core

in vec3 ObjectColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(ObjectColor, 1.0f);
}