#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColour; // colour
out vec2 texCoord; // texture coordinates

void main() {
    gl_Position = vec4(aPos, 1.0f);
    ourColour = aColour;
    texCoord = aTexCoord;
}
