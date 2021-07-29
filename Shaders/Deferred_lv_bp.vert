#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 aModel;

flat out int instanceID;
uniform mat4 model;
uniform bool instanced = true;
uniform float sphereRadius = 6.0;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat4 infiniteProj;
};

void main() {
	vec3 position = aPos / sphereRadius;
	instanceID = gl_InstanceID;
	gl_Position = infiniteProj*view*aModel*vec4(position, 1.0);
}