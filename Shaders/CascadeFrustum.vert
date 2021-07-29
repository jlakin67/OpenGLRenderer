#version 430 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat4 infiniteProj;
};

uniform mat4 inverseView;

void main() {
	gl_Position = infiniteProj * view* inverseView*vec4(aPos, 1.0f);
}