#version 430 core

layout (location = 0) out float gDepth;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gAlbedoSpec;

in vec3 viewPosition;
in vec3 pos;
in vec3 normal;
in vec2 texCoords;
in vec3 tangent;
in vec3 bitangent;

uniform bool containsDiffuse = false;
uniform bool containsSpecular = false;
uniform bool containsAmbient = false;
uniform bool containsNormal = false;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;
uniform vec4 color = vec4(1.0,0.0,0.0,1.0);

//not the most efficient, better to transform positions to tangent space in vertex shader
const mat3 TBN = mat3(tangent, bitangent, normal);

void main() {
	vec4 diffuseColor;
	if (containsDiffuse) {
		diffuseColor = texture(texture_diffuse0, texCoords);
		if (diffuseColor.a < 0.01) discard;
	} else {
		diffuseColor = color;
	}
	gAlbedoSpec.rgb = diffuseColor.rgb;
	float specularStrength;
	if (containsSpecular) {
		specularStrength = texture(texture_specular0, texCoords).r;
	} else {
		specularStrength = 0.3;
	}
	gAlbedoSpec.a = specularStrength;
	bool condition1 = all(notEqual(tangent, vec3(0,0,0)));
	bool condition2 = all(notEqual(bitangent, vec3(0,0,0)));
	//w=1 means it wil be used in lighting calculation, w=0 means it won't
	//meant to separate forward render from deferred
	vec4 newNormal = vec4(0,0,0,1);
	if (containsNormal && condition1 && condition2) {
		newNormal.xyz = TBN*vec3(texture(texture_normal0, texCoords));
	} else newNormal.xyz = normal;
	gNormal = newNormal;
	gPosition.xyz = pos;
	gPosition.w = 1;
	gDepth = -viewPosition.z;
}