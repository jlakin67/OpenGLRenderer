#version 430 core

out vec4 FragColor;
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

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	float specularExponent;
	float lightLinear;
	float lightQuadratic;
};

//not the most efficient, better to transform positions to tangent space in vertex shader
const mat3 TBN = mat3(tangent, bitangent, normal);

vec3 pointLightShading(vec3 lightPos, vec4 lightColor, vec4 diffuseColor, vec3 normal) {
	vec3 lightDir = normalize(lightPos - pos);
	float distance = length(lightPos - pos);
	float attenuation = 1 / (1.0 + (lightLinear*distance) + (lightQuadratic*pow(distance, 2.0)));
	float diffuseStrength = max(dot(lightDir, normal), 0);
	vec3 diffuse = diffuseStrength*lightColor.rgb*diffuseColor.rgb;
	vec3 viewDir = normalize(cameraPos - pos);
	vec3 halfwayDir = normalize(viewDir+lightDir);
	vec3 specularColor; vec3 specular;
	float spec = pow(max(dot(normal, halfwayDir), 0), specularExponent);
	if (containsSpecular) {
		specularColor = vec3(texture(texture_specular0, texCoords));
	} else {
		specularColor = vec3(0.3,0.3,0.3);
	}
	specular = spec*vec3(lightColor)*specularColor;
	return attenuation*(diffuse+specular);
}

void main() {
	vec3 finalColor = vec3(0,0,0);
	vec4 diffuseColor;
	if (containsDiffuse) {
		diffuseColor = texture(texture_diffuse0, texCoords);
		if (diffuseColor.a < 0.01) discard;
	} else {
		diffuseColor = color;
	}
	vec3 newNormal;
	bool condition1 = all(notEqual(tangent, vec3(0,0,0)));
	bool condition2 = all(notEqual(bitangent, vec3(0,0,0)));
	if (containsNormal && condition1 && condition2) {
		newNormal = TBN*vec3(texture(texture_normal0, texCoords));
	} else newNormal = normal;
	for (int i = 0; i < numLights; i++) {
		finalColor += pointLightShading(vec3(lightPos[i]), lightColor[i], diffuseColor, newNormal);
	}
	float ambientStrength;
	if (containsAmbient) {
		ambientStrength = texture(texture_ambient0, texCoords).r;
	} else {
		ambientStrength = 0.3;
	}
	vec3 ambient = ambientStrength*vec3(diffuseColor);
	finalColor += ambient;
	FragColor = vec4(finalColor, 1.0);
}