#version 330 core
out vec4 FragColour;
in vec3 ourColour;
in vec3 pos;

void main() {
    FragColour = vec4(pos, 1.0f);
}
