#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 viewPosition;
out vec3 pos;
out vec3 normal;
out vec2 texCoords;
out vec3 tangent;
out vec3 bitangent;

uniform mat4 model;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
};

void main() {
	vec4 worldPos = model*vec4(aPos,1.0);
	normal = normalize(mat3(inverse(transpose(model)))*aNormal);
	texCoords = aTexCoords;
	tangent = normalize(mat3(model)*aTangent);
	bitangent = normalize(mat3(model)*aBitangent);
	vec4 viewPos = view*worldPos;
	gl_Position = projection*viewPos;
	pos = vec3(worldPos);
	viewPosition = vec3(viewPos);
}