#version 460 core
in vec2 oUV;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform bool useSpecular;

uniform float shininess;

struct Light {  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light; 

uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 cameraPos;

void main()
{
    vec4 texColor = texture(texture_diffuse, oUV);
    vec3 ambient = texColor.rgb * light.ambient * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = texColor.rgb * light.diffuse * diff * lightColor;

    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 specularColor = vec3(1.0);
    if (useSpecular)
    {
        specularColor = texture(texture_specular, oUV).rgb;
    }

    float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 final = ambient + diffuse + specular * specularColor;

    FragColor = vec4(final, texColor.a);
}