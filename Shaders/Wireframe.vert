#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 aModel;
layout (location = 9) in vec4 offset;

uniform mat4 model;
uniform bool instanced = true;
uniform float sphereRadius = 6.0;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat4 infiniteProj;
};

void main() {
	mat4 newModel = model;
	vec3 newPos = aPos;
	if (instanced) {
		newModel = aModel;
		//newPos = aPos / sphereRadius;
	}
	gl_Position = infiniteProj*view*newModel*vec4(newPos, 1.0);
}