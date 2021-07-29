#version 430 core

out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 texCoords;
in vec3 tangent;
in vec3 bitangent;

uniform sampler2D texture_diffuse0;

void main() {
	vec4 color = texture(texture_diffuse0, texCoords);
	if (color.a < 0.01) discard;
	FragColor = color;
}