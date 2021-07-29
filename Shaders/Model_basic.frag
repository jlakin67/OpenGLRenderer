#version 430 core

out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 texCoords;
in vec3 tangent;
in vec3 bitangent;

uniform vec4 color;

void main() {
	FragColor = color;
}