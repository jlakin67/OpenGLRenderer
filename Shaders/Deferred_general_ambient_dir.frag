#version 430 core

//used with light volumes, ambient with a directional light

#define ONE_OVER_TWO_PI 0.1591549f

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gSpecularRoughness;
uniform sampler2D SSAO;
uniform int shadingMode = 0; //0 is blinn-phong, 1 is PBR
uniform bool hasPBR = false;
uniform bool containsShadow = false;
uniform float ambientStrength = 0.3;
uniform float shadowFar = 25.0;
uniform int numShadowCascades = 3;
uniform float shadowSplitDepths[6];
uniform sampler2DArrayShadow shadowCascades;
uniform mat4 cascadedShadowMatrices[5];

	mat4 biasMatrix = mat4(vec4(0.5, 0.0, 0.0, 0.0),
						   vec4(0.0, 0.5, 0.0, 0.0),
						   vec4(0.0, 0.0, 0.5, 0.0),
						   vec4(0.5, 0.5, 0.5, 1.0));

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	vec4 lightParam[64]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
	vec3 lightDir;
	vec4 lightDirColor; //to disable set lightDirColor to vec3(0,0,0)
};

float gaussian(float distanceSquared, float sigma) {
	return 0.3989423f*exp(-distanceSquared / (2.0f*sigma*sigma)) / sigma;
}

float cascadedShadowCalculation(float viewDepth, vec4 worldPosition, vec3 constantShadowTexCoordDx, 
vec3 constantShadowTexCoordDy, float bias, vec2 filterStep) {
	int shadowCascadeIndex = 0;
	float shadowCascadeScale = 1.0f;
	for (int i = 0; i < numShadowCascades; i++) {
		if (viewDepth > shadowSplitDepths[i] && viewDepth <= shadowSplitDepths[i+1]) {
			shadowCascadeIndex = i;
			shadowCascadeScale = (viewDepth - shadowSplitDepths[i])/(shadowSplitDepths[i+1] - shadowSplitDepths[i]);
		}
	}
	 
	mat4 shadowCascadeMatrix = cascadedShadowMatrices[shadowCascadeIndex];
	vec4 shadowTextureCoordinate = biasMatrix*shadowCascadeMatrix*worldPosition;
	//mat2 screenToShadow = mat2(constantShadowTexCoordDx.st, constantShadowTexCoordDy.st);
	//mat2 shadowToScreen = inverse(screenToShadow);
	vec2 shadowTexelStep = 1.0f / vec2(textureSize(shadowCascades, 0).xy);
	shadowTexelStep = filterStep*shadowTexelStep;
	shadowTextureCoordinate.xy += shadowTexelStep;
	//vec2 screenTexelStep = shadowToScreen*shadowTexelStep;
	//float depthDelta = constantShadowTexCoordDx.z*screenTexelStep.x +
	//constantShadowTexCoordDy.z * screenTexelStep.y;
	//bias /= (shadowCascadeIndex*shadowCascadeIndex + 3.5f);
	//shadowTextureCoordinate.z += abs(depthDelta);
	bias /= mix(1.0f, 10.0f, pow(shadowCascadeIndex / (numShadowCascades-1.0f), 2));
	shadowTextureCoordinate.z -= bias;
	shadowTextureCoordinate.xyzw = shadowTextureCoordinate.xywz; //last component is depth reference for comparison
	//second to last is the index into the texture array
	shadowTextureCoordinate.z = shadowCascadeIndex;
	float shadow = texture(shadowCascades, shadowTextureCoordinate);
	return shadow;
}

float cascadedShadowCalculationPCF(float viewDepth, vec4 worldPosition, int kernelOffset) {
	float shadow = 1.0f;

	mat4 constantShadowCascadeMatrix = cascadedShadowMatrices[numShadowCascades-1];
	vec4 constantShadowTexCoord =  biasMatrix*constantShadowCascadeMatrix*worldPosition;
	vec3 constantShadowTexCoordDx = dFdx(constantShadowTexCoord.xyz);
	vec3 constantShadowTexCoordDy = dFdy(constantShadowTexCoord.xyz);
	float depthDx = constantShadowTexCoordDx.z;
	float depthDy = constantShadowTexCoordDy.z;
	float depthSlope = sqrt(depthDx*depthDx + depthDy*depthDy);
	float bias = 0.05*depthSlope + 0.005;


	if (kernelOffset < 1) {
		shadow = cascadedShadowCalculation(viewDepth, worldPosition, constantShadowTexCoordDx, constantShadowTexCoordDy, bias, vec2(0.0f, 0.0f));
	} else {
		int kernelWidth = 1 + 2*kernelOffset;
		shadow = 0.0f;
		for (int i = -kernelOffset; i <= kernelOffset; i++) {
			for (int j = -kernelOffset; j <= kernelOffset; j++) {
				shadow += gaussian(i*i + j*j, (kernelWidth/6.0f))*
				cascadedShadowCalculation(viewDepth, worldPosition, constantShadowTexCoordDx, constantShadowTexCoordDy, bias, vec2(i, j));
			}
		}
		
		
	}

	return shadow;
}

vec3 dirLightShading(vec4 diffuseColor, vec4 specularExponent, vec3 normal, vec3 position) {
	vec3 finalColor = vec3(0.0,0.0,0.0);
	float diffuseStrength = max(dot(lightDir, normal), 0);
	finalColor += diffuseStrength * lightDirColor.rgb * diffuseColor.rgb;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(cameraPos - position);
	float spec = pow(max(dot(normal, reflectDir), 0), specularExponent.w);
	finalColor += spec * specularExponent.rgb * lightDirColor.rgb;
	return finalColor;
}

void main() {
	vec4 normal = texture(gNormal, texCoord);
	if (normal.a == 0) discard;
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	vec4 albedo = vec4(texture(gAlbedo, texCoord).rgb, 1.0f);
	vec4 specularRoughness = texture(gSpecularRoughness, texCoord);
	vec4 sampledDepth = texture(gDepth, texCoord);
	float viewDepth = sampledDepth.r;
	float shadow = 1.0;
	float ao = texture(SSAO, texCoord).r;
	if (containsShadow) {
		shadow = cascadedShadowCalculationPCF(viewDepth, position, 2);
	}
	if (shadingMode == 0) {
		vec3 finalColor = vec3(0,0,0);
		finalColor += shadow*dirLightShading(albedo, specularRoughness, normal.xyz, position.xyz);
		finalColor += ao*ambientStrength*albedo.rgb;
		FragColor = vec4(finalColor, 1.0f);
	} 
	else FragColor = vec4(0.2f, 0.2f, 0.2f, 1.0f);
}