#version 460 core
in vec2 oUV;
out vec4 FragColor;
uniform sampler2D uTex;
void main()
{
    FragColor = texture(uTex, oUV);
}