#version 430 core

#define ONE_OVER_TWO_PI 0.1591549f
#define MAX_POINT_LIGHT_DISK_RADIUS 0.1f

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gSpecularExponent;
uniform bool containsShadow = false;
uniform samplerCubeArrayShadow shadowMaps;
uniform float ambientStrength = 0.3;
uniform float shadowFar = 25.0;

uniform int shadingMode = 0; //0 is blinn-phong

uniform vec2 windowSize = vec2(1600, 900);
flat in int instanceID;
flat in mat4 light_model;

uniform sampler3D noise;

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	vec4 lightParam[64]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
	vec3 lightDir;
	vec4 lightDirColor;
};

layout (std140, binding = 3) uniform Samples {
	vec2 poissonDisk[81]; //.1 disk radius, between 0 and 1
	vec3 poissonDiskSphere[81]; //.05 disk radius
	vec3 poissonDiskHemisphere[81]; //.025 disk radius
};

float isShadow(samplerCubeArrayShadow shadowMaps, vec4 texCoord, float depth) {
	float shadow = 1.0;
	if (depth > 1.0) return shadow;
	shadow = texture(shadowMaps, texCoord, depth);
	return shadow;
}

float pointLightPoissonDisk(samplerCubeArrayShadow shadowMaps, vec4 shadowTexCoord, float depth, float diskRadius, int numSamples, vec4 worldPosition) {
	float shadow = 0.0f;

	for (int i = 0; i < numSamples; i++) {
			vec2 randomAngle = texture(noise, worldPosition.xyz).xy;
			vec3 jitter = vec3(cos(randomAngle.x)*sin(randomAngle.y),
							   sin(randomAngle.x)*sin(randomAngle.y),
							   (randomAngle.y)
							  );
			shadow +=
			isShadow(shadowMaps, vec4(shadowTexCoord.xyz + diskRadius*length(poissonDisk[i])*(poissonDiskSphere[i] + jitter), shadowTexCoord.w), depth);
	}
	shadow /= float(numSamples);
	return shadow;
}

vec3 pointLightShading(vec3 lightPos, vec4 lightColor, vec4 diffuseColor, vec3 normal, 
vec4 specularExponent, vec3 position, vec4 param) {
	float lightConstant = param.x;
	float lightLinear = param.y;
	float lightQuadratic = param.z;
	vec3 lightDir = normalize(lightPos - position);
	float distance = length(lightPos - position);
	float attenuation = 1 / (lightConstant + (lightLinear*distance) + (lightQuadratic*distance*distance));
	float diffuseStrength = max(dot(lightDir, normal), 0);
	vec3 diffuse = diffuseStrength*lightColor.rgb*diffuseColor.rgb;
	vec3 viewDir = normalize(cameraPos - position);
	vec3 halfwayDir = normalize(viewDir+lightDir);
	float spec = pow(max(dot(normal, halfwayDir), 0), specularExponent.w);
	vec3 specular;
	specular = spec*specularExponent.rgb*vec3(lightColor);
	return attenuation*(diffuse+specular);
}

void main() {
	vec2 texCoord = vec2(gl_FragCoord.x/windowSize.x, gl_FragCoord.y/windowSize.y);
	vec4 normal = texture(gNormal, texCoord);
	if (normal.a == 0) discard;
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	if (length(position - light_model[3]) > light_model[0][0]) discard;
	vec4 albedo = vec4(texture(gAlbedo, texCoord).rgb, 1.0f);
	vec4 specularExponent = texture(gSpecularExponent, texCoord);
	float shadow = 1.0;
	if (containsShadow) {
		vec4 shadowTexCoord = vec4(position.xyz - lightPos[instanceID].xyz, instanceID);
		float depth = length(shadowTexCoord.xyz) / shadowFar;
		float dfdx = dFdx(depth);
		float dfdy = dFdy(depth);
		float depthSlope = sqrt(dfdx*dfdx + dfdy*dfdy);
		float bias = 0.01*depthSlope + 0.015;
		float diskRadius = 0.04f;
		diskRadius = clamp(diskRadius, 0.0f, MAX_POINT_LIGHT_DISK_RADIUS);
		int numSamples = int(mix(1, 81, pow( (diskRadius/MAX_POINT_LIGHT_DISK_RADIUS), 1.0f/2.2f) ));
		bias += 0.75*diskRadius;
		shadow = pointLightPoissonDisk(shadowMaps, shadowTexCoord, depth-bias, diskRadius, numSamples, position);
	}
	if (shadingMode == 0) {
		vec3 finalColor = vec3(0,0,0);
		vec4 param = lightParam[instanceID];
		finalColor += shadow*pointLightShading(lightPos[instanceID].xyz, lightColor[instanceID], albedo, 
		normal.xyz, specularExponent, position.xyz, param);
		FragColor = vec4(finalColor, 1.0);
	} else FragColor = vec4(0.2f, 0.2f, 0.2f, 1.0f);
}