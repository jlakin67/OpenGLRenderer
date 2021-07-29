#version 430 core

//without light volumes, blinn-phong

out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform bool containsShadow = false;
uniform samplerCubeArrayShadow shadowMaps;
uniform float ambientStrength = 0.3;
uniform float far = 25.0;

in vec2 texCoord;

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	vec4 lightParam[64]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
	vec3 lightDir;
	vec4 lightDirColor;
};

vec3 pointLightShading(vec3 lightPos, vec4 lightColor, vec4 diffuseColor, vec3 normal, 
float specularStrength, vec3 position, vec4 param) {
	float lightConstant = param.x;
	float lightLinear = param.y;
	float lightQuadratic = param.z;
	float specularExponent = param.w;
	vec3 lightDir = normalize(lightPos - position);
	float distance = length(lightPos - position);
	float attenuation = 1 / (lightConstant + (lightLinear*distance) + (lightQuadratic*distance*distance));
	float diffuseStrength = max(dot(lightDir, normal), 0);
	vec3 diffuse = diffuseStrength*lightColor.rgb*diffuseColor.rgb;
	vec3 viewDir = normalize(cameraPos - position);
	vec3 halfwayDir = normalize(viewDir+lightDir);
	float spec = pow(max(dot(normal, halfwayDir), 0), specularExponent);
	vec3 specular;
	specular = spec*specularStrength*vec3(lightColor);
	return attenuation*(diffuse+specular);
}

float isShadow(samplerCubeArrayShadow shadowMaps, vec4 texCoord) {
	float depth = length(texCoord.xyz) / far;
	float shadow = 1.0;
	if (depth > 1.0) return shadow;
	shadow = texture(shadowMaps, texCoord, depth-0.03);
	return shadow;
}

void main() {
	vec4 normal = texture(gNormal, texCoord);
	if (normal.a == 0) discard;
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	vec4 albedoSpec = texture(gAlbedoSpec, texCoord);
	vec4 diffuse = vec4(albedoSpec.rgb, 1.0);
	vec3 finalColor = vec3(0,0,0);
	for (int i = 0; i < numLights; i++) {
		float shadow = 1.0;
		if (containsShadow) {
			vec4 shadowTexCoord = vec4(position.xyz - lightPos[i].xyz, i);
			shadow = isShadow(shadowMaps, shadowTexCoord);
		}
		vec4 param = lightParam[i];
		finalColor += shadow*pointLightShading(lightPos[i].xyz, lightColor[i], diffuse, 
		normal.xyz, albedoSpec.a, position.xyz, param);
	}
	finalColor.rgb += ambientStrength*diffuse.rgb;
	FragColor = vec4(finalColor,1.0);
}