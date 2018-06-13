#version 330 core
out vec4 FragColour;
in vec3 ourColour;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    FragColour = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2) * vec4(ourColour, 1.0);
}
