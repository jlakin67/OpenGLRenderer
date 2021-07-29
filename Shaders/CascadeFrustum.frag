#version 430 core

out vec4 FragColor;

void main() {
	int index = int(floor(gl_PrimitiveID / 12)) % 5;
	vec4 color[] = {vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(1,1,0,0), vec4(1,0,1,0)};
	FragColor = color[index];
}