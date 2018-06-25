#version 330 core
out vec4 FragColour;
in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;

uniform float alpha;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position; // not needed since we calculate lighting in view space
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main() {
    // ambient
    vec3 ambient = material.ambient * light.ambient;
    
    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;
    
    // specular
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (spec * material.specular) * light.specular;
    
    vec3 res = ambient + diffuse + specular;
    FragColour = vec4(res, alpha);
}
