#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
};

uniform mat4 model;

out VS_GS_Interface 
{
	vec2 texCoord;
	//float texBias;
} vs_out;

void main() {
	vs_out.texCoord = aTexCoord;
	//vec4 screenSpaceCoord = projection*view*model*vec4(aPos, 1.0f);
	//screenSpaceCoord /= screenSpaceCoord.w;
	//vec2 stepResult1 = step(1.0f, screenSpaceCoord.xy);
	//vec2 stepResult2 = step(-1.0f, screenSpaceCoord.xy);
	//stepResult2 = vec2(1.0f, 1.0f) - stepResult2;
	//vs_out.texBias = -(dot(stepResult1, vec2(1.0f, 1.0f)) + dot(stepResult1, vec2(1.0f, 1.0f)));
	gl_Position = model*vec4(aPos, 1.0);
}