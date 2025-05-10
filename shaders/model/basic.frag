#version 460 core

#define MAX_LIGHTS 16

in vec2 oUV;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform bool useSpecular;
uniform float shininess;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct Light {  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 lightColor;
    vec3 lightPos;
    float range;
};
uniform Light lights[MAX_LIGHTS]; 
uniform int numLights;

uniform vec3 cameraPos;

vec3 calculateLight(Light light, vec4 texColor){
    vec3 ambient = texColor.rgb * light.ambient * light.lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.lightPos - FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = texColor.rgb * light.diffuse * diff * light.lightColor;

    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 specularColor = vec3(1.0);
    if (useSpecular)
        specularColor = texture(texture_specular, oUV).rgb;
    
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 final = ambient + diffuse + specular * specularColor;
    return final;
}


void main()
{
    vec4 texColor = texture(texture_diffuse, oUV);
    vec3 finalColor = vec3(0.0);
    for(int i = 0; i < numLights; i++) {
        finalColor += calculateLight(lights[i], texColor);
    }

    FragColor = vec4(finalColor, texColor.a);
}