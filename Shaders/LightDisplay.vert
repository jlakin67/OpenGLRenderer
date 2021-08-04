#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 aModel;
layout (location = 9) in vec4 offset;

flat out int instanceID;
uniform mat4 model;
uniform bool instanced = false;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
};

void main() {
	
	mat4 newModel = model;
	if (instanced) {
		newModel[3] = offset;
	}
	instanceID = gl_InstanceID;
	
	gl_Position = projection*view*newModel*vec4(aPos, 1.0);
}