#version 430 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D tex;

void main() {
	vec4 color = texture(tex, texCoord);
	if (color.a < 0.001) discard;
	FragColor = color;
}