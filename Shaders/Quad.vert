#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out float depth;
out vec2 texCoord;
out vec3 normal;
out vec3 pos;
out vec3 viewPosition;

uniform mat4 model;
uniform vec3 normal_in;
uniform float texScale = 1.0f;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
};

void main() {
	vec4 worldPos = model*vec4(aPos,1.0);
	vec4 viewPos = view*worldPos;
	gl_Position = projection*viewPos;
	pos = vec3(worldPos);
	texCoord = texScale*aTexCoord;
	normal = normalize(mat3(inverse(transpose(model)))*normal_in);
	viewPosition = vec3(viewPos);
}