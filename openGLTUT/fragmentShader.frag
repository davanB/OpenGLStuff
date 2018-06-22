#version 330 core
out vec4 FragColour;
in vec3 normal;
in vec3 fragPos;

uniform float alpha;
uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;

void main() {
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColour;
    
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;
    
    vec3 res = (ambient + diffuse) * objectColour;
    FragColour = vec4(res, alpha);
}
