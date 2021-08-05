#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

out VS_GS_Interface 
{
	vec2 texCoord;
} vs_out;

void main() {
	vs_out.texCoord = aTexCoord;
	gl_Position = model*vec4(aPos, 1.0);
}