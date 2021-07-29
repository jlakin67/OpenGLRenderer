#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;
uniform mat3 model = mat3(2.0, 0.0, 0.0,
						  0.0, 2.0, 0.0,
						  0.0, 0.0, 1.0
						 );

void main() {
	texCoord = aTexCoord;
	vec3 newPos = model*aPos;
	gl_Position = vec4(newPos, 1.0);
}