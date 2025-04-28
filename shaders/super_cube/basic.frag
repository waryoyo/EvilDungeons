#version 460 core
in vec2 oUV;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light; 

uniform sampler2D uTex;

uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 cameraPos;

void main()
{
    vec3 ambient = light.ambient * material.ambient * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diff * lightColor;

    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec * lightColor;  

    vec3 result = ambient + diffuse + specular;
    vec4 texColor = texture(uTex, oUV); 

    FragColor = vec4(texColor.rgb * result, texColor.a);
}