#version 330 core
out vec4 FragColour;

uniform float alpha;
uniform vec3 objectColour;
uniform vec3 lightColour;

void main() {
    FragColour = vec4(objectColour * lightColour, alpha);
}
