#version 430 core

layout (location = 0) out float gDepth;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gAlbedo;
layout (location = 4) out vec4 gSpecularRoughness;

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
uniform bool containsAlpha = false;
uniform bool containsRoughness = false;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_ambient0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;
uniform sampler2D texture_alpha;
uniform sampler2D texture_roughness;
uniform vec4 color = vec4(0.6f,0.6f,0.6f,1.0f);
uniform float specularHighlight = 50.0f;
uniform vec3 specularColor = vec3(0.0f, 0.0f, 0.0f);

const mat3 TBN = mat3(tangent, bitangent, normal);

void main() {
	vec4 diffuseAlpha = color;
	if (containsDiffuse) {
		diffuseAlpha = texture(texture_diffuse0, texCoords);
	}
	if (containsAlpha) {
		diffuseAlpha.a = texture(texture_alpha, texCoords).r;
	}
	if (diffuseAlpha.a < 0.01) discard;
	gAlbedo = vec4(diffuseAlpha.rgb, 1.0f);
	vec3 specularColorOut;
	if (containsSpecular) {
		specularColorOut = texture(texture_specular0, texCoords).rgb;
	} else {
		specularColorOut = specularColor;
	}
	float roughnessOut;
	if (containsRoughness) {
		roughnessOut = texture(texture_roughness, texCoords).r;
	} else {
		roughnessOut = specularHighlight;
	}
	gSpecularRoughness = vec4(specularColorOut, roughnessOut);
	//bool condition1 = all(equal(tangent, vec3(0,0,0)));
	//bool condition2 = all(equal(bitangent, vec3(0,0,0)));
	//w=1 means it wil be used in lighting calculation, w=0 means it won't
	//meant to separate forward render from deferred
	vec4 newNormal = vec4(0,0,0,1);
	if (containsNormal) {
		newNormal.xyz = TBN*vec3(normalize(texture(texture_normal0, texCoords)));
	} else newNormal.xyz = normal;
	gNormal = newNormal;
	gPosition.xyz = pos;
	gPosition.w = 1;
	gDepth = -viewPosition.z;
}