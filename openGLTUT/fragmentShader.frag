#version 330 core
out vec4 FragColour;
in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;

uniform float alpha;
uniform vec3 objectColour;
uniform vec3 lightColour;

void main() {
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColour;
    
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;
    
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColour;
    
    vec3 res = (ambient + diffuse + specular) * objectColour;
    FragColour = vec4(res, alpha);
}
