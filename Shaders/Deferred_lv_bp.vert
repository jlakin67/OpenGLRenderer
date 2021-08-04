#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 aModel;

flat out int instanceID;
flat out mat4 light_model;
uniform bool instanced = true;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat4 infiniteProj;
};

void main() {
	instanceID = gl_InstanceID;
	light_model = aModel;
	gl_Position = infiniteProj*view*aModel*vec4(aPos, 1.0);
}