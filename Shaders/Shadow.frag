#version 430 core

out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gDepth;
uniform samplerCubeArrayShadow shadowMaps;
uniform sampler2D gNormal;
uniform float shadowFar = 25.0;
uniform bool containsShadow = false;
uniform int numShadowCascades = 3;
uniform float shadowSplitDepths[6];
uniform sampler2DArrayShadow shadowCascades;
uniform mat4 cascadedShadowMatrices[5];

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

mat4 biasMatrix = mat4(vec4(0.5, 0.0, 0.0, 0.0),
					   vec4(0.0, 0.5, 0.0, 0.0),
					   vec4(0.0, 0.0, 0.5, 0.0),
					   vec4(0.5, 0.5, 0.5, 1.0));

float isShadow(samplerCubeArrayShadow shadowMaps, vec4 texCoord, float depth) {
	float shadow = 1.0; 
	if (depth > 1.0) return shadow;
	shadow = texture(shadowMaps, texCoord, depth);
	return shadow;
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
	shadowTextureCoordinate.z -= bias;
	shadowTextureCoordinate.xyzw = shadowTextureCoordinate.xywz; //last component is depth reference for comparison
	//second to last is the index into the texture array
	shadowTextureCoordinate.z = shadowCascadeIndex;
	float shadow = texture(shadowCascades, shadowTextureCoordinate);
	return shadow;
	/*
	float shadow2;
	
	if (shadowCascadeScale < 0.1f || shadowCascadeScale >= 0.9f) {
		vec4 shadowTextureCoordinate2;
		if (shadowCascadeIndex == numShadowCascades-1 && shadowCascadeScale >=0.5f) {
			shadowTextureCoordinate2 = biasMatrix*cascadedShadowMatrices[shadowCascadeIndex-1]*worldPosition;
			shadowTextureCoordinate2.z -= bias;
			shadowTextureCoordinate2.xyzw = shadowTextureCoordinate2.xywz;
			shadowTextureCoordinate.z = shadowCascadeIndex-1;
			shadow2 = texture(shadowCascades, shadowTextureCoordinate2);
			//return min(shadow, shadow2);
			return shadowCascadeScale*shadow + (1.0f - shadowCascadeScale)*shadow2;
		}
		else if (shadowCascadeIndex == 0 && shadowCascadeScale <= 0.5f) {
			shadowTextureCoordinate2 = biasMatrix*cascadedShadowMatrices[shadowCascadeIndex+1]*worldPosition;
			shadowTextureCoordinate2.z -= bias;
			shadowTextureCoordinate2.xyzw = shadowTextureCoordinate2.xywz;
			shadowTextureCoordinate.z = shadowCascadeIndex+1;
			shadow2 = texture(shadowCascades, shadowTextureCoordinate2);
			//return min(shadow, shadow2);
			return (1.0f - shadowCascadeScale)*shadow2 + shadowCascadeScale*shadow;
		} else if (shadowCascadeScale <= 0.5f) {
			shadowTextureCoordinate2 = biasMatrix*cascadedShadowMatrices[shadowCascadeIndex-1]*worldPosition;
			shadowTextureCoordinate2.z -= bias;
			shadowTextureCoordinate2.xyzw = shadowTextureCoordinate2.xywz;
			shadowTextureCoordinate.z = shadowCascadeIndex-1;
			shadow2 = texture(shadowCascades, shadowTextureCoordinate2);
			//return min(shadow, shadow2);
			return shadowCascadeScale*shadow + (1.0f - shadowCascadeScale)*shadow2;
		} else if (shadowCascadeScale > 0.5f) {
			shadowTextureCoordinate2 = biasMatrix*cascadedShadowMatrices[shadowCascadeIndex+1]*worldPosition;
			shadowTextureCoordinate2.z -= bias;
			shadowTextureCoordinate2.xyzw = shadowTextureCoordinate2.xywz;
			shadowTextureCoordinate.z = shadowCascadeIndex+1;
			shadow2 = texture(shadowCascades, shadowTextureCoordinate2);
			//return min(shadow, shadow2);
			return (1.0f - shadowCascadeScale)*shadow + shadowCascadeScale*shadow2;
		}
	} else return shadow;

	/*
	if (kernelOffset < 1) {
		float shadow = texture(shadowCascades, shadowTextureCoordinate);
		return shadow;
	} else {
		int kernelWidth = 1 + 2*kernelOffset;
		float shadow = 0.0f;
		vec2 texelStep = 1.0f / vec2(textureSize(shadowCascades, 0).xy);
		for (int i = -kernelOffset; i <= kernelOffset; i++) {
			for (int j = -kernelOffset; j <= kernelOffset; j++) {
				vec4 sampleTextureCoord = vec4(shadowTextureCoordinate.x + float(i)*texelStep.x,
				shadowTextureCoordinate.y + float(j)*texelStep.y,
				shadowCascadeIndex,
				projectedDepth - 10*bias);
				shadow += texture(shadowCascades, sampleTextureCoord);
			}
		}
		shadow /= float(kernelWidth*kernelWidth);
		return shadow;
		
	}
	*/
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
				shadow += cascadedShadowCalculation(viewDepth, worldPosition, constantShadowTexCoordDx, constantShadowTexCoordDy, bias, vec2(i, j));
			}
		}
		shadow /= float(kernelWidth*kernelWidth);
		
	}

	return shadow;
}


void main() {
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	vec4 sampledDepth = texture(gDepth, texCoord);
	float viewDepth = sampledDepth.r;
	vec3 finalColor = vec3(1,1,1);
	
	//point light shadow calculation
	/*
	for (int i = 0; i < numLights; i++) {
		float shadow = 1.0;
		if (containsShadow) {
			vec4 shadowTexCoord = vec4(position.xyz - lightPos[i].xyz, i);
			float depth = length(shadowTexCoord.xyz) / shadowFar;
			float dfdx = dFdx(depth);
			float dfdy = dFdy(depth);
			float depthSlope = sqrt(dfdx*dfdx + dfdy*dfdy);
			float bias = 0.01*depthSlope + 0.015;
			shadow = 1 - isShadow(shadowMaps, shadowTexCoord, depth-bias);
		}
		finalColor -= vec3(shadow);
	}
	*/
	if (containsShadow) {
		//cascaded shadow calculation
		float shadow = 1.0;
		finalColor -= (1 - vec3(cascadedShadowCalculationPCF(viewDepth, position, 0)));
	}
	FragColor = vec4(finalColor, 1.0);
}