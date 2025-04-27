#version 460 core
in vec2 oUV;
out vec4 FragColor;

const int MAX_DIFFUSE  = 8;
const int MAX_SPECULAR = 4;
const int MAX_NORMAL = 4; 
const int MAX_HEIGHT = 2;

uniform sampler2D texture_diffuse[MAX_DIFFUSE];
uniform sampler2D texture_specular[MAX_SPECULAR];
uniform sampler2D texture_normal[MAX_SPECULAR];
uniform sampler2D texture_height[MAX_SPECULAR];

uniform int diffuseCount;
uniform int specularCount;
uniform int normalCount;
uniform int heightCount;

void main()
{
    FragColor = texture(uTex, oUV);
}