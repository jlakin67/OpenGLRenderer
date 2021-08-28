#version 430 core

#define ONE_OVER_TWO_PI 0.1591549f
#define ONE_OVER_PI 0.3183010f
#define MAX_POINT_LIGHT_DISK_RADIUS 0.1f

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gSpecularRoughness;
uniform sampler2D SSAO;
uniform bool containsShadow = false;
uniform samplerCubeArrayShadow shadowMaps;
uniform float ambientStrength = 0.3;
uniform float shadowFar = 25.0;

uniform int shadingMode = 0; //0 is blinn-phong, 1 is isotropic BRDF

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
			vec3 jitter = diskRadius*vec3(cos(randomAngle.x)*sin(randomAngle.y),
							   sin(randomAngle.x)*sin(randomAngle.y),
							   (randomAngle.y)
							  );
			shadow +=
			isShadow(shadowMaps, vec4(shadowTexCoord.xyz + diskRadius*length(poissonDisk[i])*(poissonDiskSphere[i] + jitter), shadowTexCoord.w), depth);
			//shadow +=
			//isShadow(shadowMaps, vec4(shadowTexCoord.xyz + diskRadius*length(poissonDisk[i])*(poissonDiskSphere[i]), shadowTexCoord.w), depth);
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
	float dist = length(lightPos - position);
	float NdotL = dot(lightDir, normal);
	float attenuation = 1.0f / (lightConstant + (lightLinear*dist) + (lightQuadratic*dist*dist));
	float diffuseStrength = max(NdotL, 0);
	vec3 diffuse = diffuseStrength*lightColor.rgb*diffuseColor.rgb;
	vec3 viewDir = normalize(cameraPos - position);
	vec3 halfwayDir = normalize(viewDir+lightDir);
	float spec;
	if (NdotL < 0.0f) spec = 0.0f;
	else spec = pow(max(dot(normal, halfwayDir), 0), specularExponent.w);
	vec3 specular;
	specular = spec*specularExponent.rgb*vec3(lightColor);
	return attenuation*(diffuse+specular);
}

float GGXDistribution(vec3 normal, vec3 h, float roughness) {
	float alpha = roughness*roughness;
	float NdotH = max(dot(normal, h), 0.0f);
	float alpha_squared = alpha*alpha;
	float denom = (NdotH*NdotH)*(alpha_squared - 1.0f) + 1.0f;
	denom = denom*denom;
	float num = ONE_OVER_PI * alpha_squared;
	return num / denom;
}

vec3 fresnelSchlick(vec3 F0, float VdotH) {
	return F0 + (1.0f - F0)*pow(max(1.0f - VdotH, 0.0f), 5.0f);
}

float SmithG1(float NdotV, float k) {
	float denom = NdotV*(1.0f - k) + k;
	return NdotV / denom;
}

float SchlickSmithG2(float NdotV, float NdotL, float roughness) {
	float r = (roughness + 1.0f) / 2.0f;
	float k = (r + 1.0f);
	k = 0.125f * k * k;
	return SmithG1(NdotV, k) * SmithG1(NdotL, k);
}

vec3 pointLightShadingPBR(vec3 albedo, float roughness, float metalness, vec3 normal, 
						vec3 pos, vec3 lightPos, vec3 cameraPos, vec3 lightColor, vec4 param) {
	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metalness);
	vec3 V = normalize(cameraPos - pos);
	vec3 L = normalize(lightPos - pos);
	float NdotV = max(dot(normal, V), 0.0f);
	float NdotL = max(dot(normal, L), 0.0f);
	vec3 h = normalize(L + V);
	vec3 kS = fresnelSchlick(F0, max(dot(V, h), 0.0f));
	vec3 num = kS*GGXDistribution(normal, h, roughness)*SchlickSmithG2(NdotV, NdotL, roughness);
	float denom = 4.0f*NdotV*NdotL;
	vec3 spec = num / max(denom, 0.00390625f);
	vec3 kD = 1.0f - kS;
	kD *= (1.0f - metalness);
	float lightConstant = param.x;
	float lightLinear = param.y;
	float lightQuadratic = param.z;
	float dist = length(lightPos - pos);
	float attenuation = 1.0f / (lightConstant + (lightLinear*dist) + (lightQuadratic*dist*dist));
	return (kD*albedo*ONE_OVER_PI + spec)*lightColor*NdotL*attenuation;
}

void main() {
	vec2 texCoord = vec2(gl_FragCoord.x/windowSize.x, gl_FragCoord.y/windowSize.y);
	vec4 normal = normalize(texture(gNormal, texCoord));
	if (normal.a == 0) discard; //a == 0 to indicate it's from a forward rendered screen pixel
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	float volumeScale = 1.0f - smoothstep((7.0/8.0)*light_model[0][0], light_model[0][0], length(position - light_model[3]));
	vec4 albedo = vec4(texture(gAlbedo, texCoord).rgb, 1.0f);
	vec4 specularRoughness = texture(gSpecularRoughness, texCoord);
	float shadow = 1.0;
	//float ao = texture(SSAO, texCoord).r;
	if (containsShadow) {
		vec4 shadowTexCoord = vec4(position.xyz - lightPos[instanceID].xyz, instanceID);
		float depth = length(shadowTexCoord.xyz) / shadowFar;
		float dfdx = dFdx(depth);
		float dfdy = dFdy(depth);
		float depthSlope = sqrt(dfdx*dfdx + dfdy*dfdy);
		float bias = 0.01*depthSlope + 0.015;
		float diskRadius = 0.01f;
		diskRadius = clamp(diskRadius, 0.0f, MAX_POINT_LIGHT_DISK_RADIUS);
		int numSamples = int(mix(1, 81, pow( (diskRadius/MAX_POINT_LIGHT_DISK_RADIUS), 1.0f/2.2f) ));
		bias += 0.75*diskRadius;
		shadow = pointLightPoissonDisk(shadowMaps, shadowTexCoord, depth-bias, diskRadius, numSamples, position);
	}
	if (shadingMode == 0) {
		vec3 finalColor = vec3(0,0,0);
		vec4 param = lightParam[instanceID];
		finalColor += volumeScale*shadow*pointLightShading(lightPos[instanceID].xyz, lightColor[instanceID], albedo, 
		normal.xyz, specularRoughness, position.xyz, param);
		FragColor = vec4(finalColor, 1.0);
	} else if (shadingMode == 1) {
		vec3 finalColor = vec3(0,0,0);
		vec4 param = lightParam[instanceID];
		float roughness = specularRoughness.a;
		if (roughness > 1.0f) roughness = 1.252495f*pow(roughness, -0.31772f); //eyeballing phong exponent to roughness value
		finalColor += volumeScale*shadow*pointLightShadingPBR(albedo.rgb, roughness, specularRoughness.r, normal.xyz,
															  position.xyz, lightPos[instanceID].xyz, cameraPos, lightColor[instanceID].rgb,
															  param);
		FragColor = vec4(finalColor, 1.0);
	} else FragColor = vec4(0.2f, 0.2f, 0.2f, 1.0f);
}