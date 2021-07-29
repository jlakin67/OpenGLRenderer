#version 430 core

layout (location = 0) out float gDepth;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gAlbedoSpec;

in vec2 texCoord;
in vec3 normal;
in vec3 pos;
in vec3 viewPosition;
uniform sampler2D tex;

void main() {
	vec3 newNormal = normal;
	if (!gl_FrontFacing) newNormal *= -1;
	vec4 diffuseColor = texture(tex, texCoord);
	if (diffuseColor.a < 0.01) discard;
	float specularStrength = 0.3;
	gAlbedoSpec.rgb = diffuseColor.rgb;
	gAlbedoSpec.a = specularStrength;
	gNormal.xyz = newNormal;
	gNormal.w = 1;
	gPosition.xyz = pos;
	gPosition.w = 1;
	gDepth = -viewPosition.z;
}