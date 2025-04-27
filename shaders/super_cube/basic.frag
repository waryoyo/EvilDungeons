#version 460 core
in vec2 oUV;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D uTex;

uniform vec3 lightColor;
uniform vec3 lightPos; 

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse);
    vec4 texColor = texture(uTex, oUV); 

    FragColor = vec4(texColor.rgb * result, texColor.a);
}