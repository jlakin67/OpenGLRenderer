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

float isShadow(samplerCubeArrayShadow shadowMaps, vec4 texCoord, float depth) {
	float shadow = 1.0; 
	if (depth > 1.0) return shadow;
	shadow = texture(shadowMaps, texCoord, depth);
	return shadow;
}

float cascadedShadowCalculation(float viewDepth, vec4 worldPosition) {
	int shadowCascadeIndex = 0;
	float shadowCascadeScale = 1.0f;
	for (int i = 0; i < numShadowCascades; i++) {
		if (viewDepth > shadowSplitDepths[i] && viewDepth <= shadowSplitDepths[i+1]) {
			shadowCascadeIndex = i;
			shadowCascadeScale = (viewDepth - shadowSplitDepths[i])/(shadowSplitDepths[i+1] - shadowSplitDepths[i]);
		}
	}
	
	mat4 biasMatrix = mat4(vec4(0.5, 0.0, 0.0, 0.0),
						   vec4(0.0, 0.5, 0.0, 0.0),
						   vec4(0.0, 0.0, 0.5, 0.0),
						   vec4(0.5, 0.5, 0.5, 1.0));
	mat4 constantShadowCascadeMatrix = cascadedShadowMatrices[0];
	mat4 shadowCascadeMatrix = cascadedShadowMatrices[shadowCascadeIndex];
	vec4 shadowTextureCoordinate = biasMatrix*shadowCascadeMatrix*worldPosition;
	vec4 constantShadowTexCoord =  biasMatrix*constantShadowCascadeMatrix*worldPosition;
	float projectedDepth = constantShadowTexCoord.z;
	float dfdx = dFdx(projectedDepth);
	float dfdy = dFdy(projectedDepth);
	float depthSlope = sqrt(dfdx*dfdx + dfdy*dfdy);
	
	//float depthSlope = 1.0f - max(dot(normalize(normal), lightDir), 0.0f);
	float bias = 0.05*depthSlope + 0.005;
	//bias /= (shadowCascadeIndex*shadowCascadeIndex + 3.5f);
	shadowTextureCoordinate.z -= bias;
	shadowTextureCoordinate.xyzw = shadowTextureCoordinate.xywz; //last component is depth reference for comparison
	//second to last is the index into the texture array
	shadowTextureCoordinate.z = shadowCascadeIndex;
	float shadow = texture(shadowCascades, shadowTextureCoordinate);

	float shadow2;
	/*
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
	}
	*/
	return shadow;
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
		finalColor -= (1 - vec3(cascadedShadowCalculation(viewDepth, position)));
	}
	FragColor = vec4(finalColor, 1.0);
}