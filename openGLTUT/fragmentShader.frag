#version 330 core
out vec4 FragColour;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float alpha;

void main() {
    FragColour = mix(texture(texture1, texCoord), texture(texture2, texCoord), alpha);
}
