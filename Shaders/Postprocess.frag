#version 430 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D screen;

void main() {
	FragColor = texture(screen, texCoord);
}