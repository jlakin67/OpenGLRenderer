#version 430 core

in vec3 TexCoord;
out vec4 FragColor;

uniform samplerCube cubemap;

void main() {
	vec3 leftHand = TexCoord;
	leftHand.z = leftHand.z;
	FragColor = texture(cubemap, leftHand);
}