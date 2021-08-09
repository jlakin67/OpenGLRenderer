#version 430 core

#define ONE_OVER_TWO_PI 0.1591549f
#define MAX_POINT_LIGHT_DISK_RADIUS 0.1f
#define MAX_DIRECTIONAL_LIGHT_FILTER_WIDTH 12.0f

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
uniform sampler3D noise;
uniform int screenWidth = 1600;
uniform int screenHeight = 900;
uniform int noiseSize = 64;

in vec2 texCoord;

vec2 noiseTexScale  = vec2(float(screenWidth)/float(noiseSize), float(screenHeight)/float(noiseSize));

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

mat4 biasMatrix = mat4(vec4(0.5, 0.0, 0.0, 0.0),
					   vec4(0.0, 0.5, 0.0, 0.0),
					   vec4(0.0, 0.0, 0.5, 0.0),
					   vec4(0.5, 0.5, 0.5, 1.0));

float gaussian(float x, float v) {
	return ONE_OVER_TWO_PI * (1.0f / v) * exp(-x/(2.0f*v));
}

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
	vec2 shadowTexelStep = 1.0f / vec2(1024 * 4);
	shadowTexelStep = filterStep*shadowTexelStep;
	shadowTextureCoordinate.xy += shadowTexelStep;
	//experiment
	/*
	mat2 screenToShadow = mat2(constantShadowTexCoordDx.xy, constantShadowTexCoordDy.xy);
	mat2 shadowToScreen = inverse(screenToShadow);
	vec2 screenTexelStep = shadowToScreen*shadowTexelStep;
	float depthDelta = constantShadowTexCoordDx.z*screenTexelStep.x +
	constantShadowTexCoordDy.z * screenTexelStep.y;
	shadowTextureCoordinate.z += abs(depthDelta);
	*/
	//\experiment
	bias /= mix(1.0f, 10.0f, pow(shadowCascadeIndex / (numShadowCascades-1.0f), 2));
	bias += 0.00025*length(filterStep);
	shadowTextureCoordinate.z -= bias;
	shadowTextureCoordinate.xyzw = shadowTextureCoordinate.xywz; //last component is depth reference for comparison
	//second to last is the index into the texture array
	shadowTextureCoordinate.z = shadowCascadeIndex;
	float shadow = texture(shadowCascades, shadowTextureCoordinate);
	return shadow;
	
	float shadow2;
	/*
	if (shadowCascadeScale < 0.3f || shadowCascadeScale >= 0.7f) {
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
		float kernelWidth = 1 + 2*kernelOffset;
		shadow = 0.0f;
		for (int i = -kernelOffset; i <= kernelOffset; i++) {
			for (int j = -kernelOffset; j <= kernelOffset; j++) {
				shadow += gaussian(length(vec2(i,j)), (kernelWidth/6.0f)*(kernelWidth/6.0f))*
				cascadedShadowCalculation(viewDepth, worldPosition, constantShadowTexCoordDx, constantShadowTexCoordDy, bias, vec2(i, j));
			}
		}
		
		
	}

	return shadow;
}

float cascadedShadowCalculationPoissonDisk(float viewDepth, vec4 worldPosition, float searchWidth, int numSamples) {
	mat4 constantShadowCascadeMatrix = cascadedShadowMatrices[numShadowCascades-1];
	vec4 constantShadowTexCoord =  biasMatrix*constantShadowCascadeMatrix*worldPosition;
	vec3 constantShadowTexCoordDx = dFdx(constantShadowTexCoord.xyz);
	vec3 constantShadowTexCoordDy = dFdy(constantShadowTexCoord.xyz);
	float depthDx = constantShadowTexCoordDx.z;
	float depthDy = constantShadowTexCoordDy.z;
	float depthSlope = sqrt(depthDx*depthDx + depthDy*depthDy);
	float bias = 0.05*depthSlope + 0.01;
	float shadow = 0.0f;
	float randomAngle = texture(noise, worldPosition.xyz).r;
	vec2 shadowTexelStep = 1.0f / vec2(1024 * 4);
	for (int i = 0; i < numSamples; i++) {
		vec2 poissonSample = poissonDisk[i];
		poissonSample = vec2(cos(randomAngle)*poissonSample.x - sin(randomAngle)*poissonSample.y,
							 sin(randomAngle)*poissonSample.x + cos(randomAngle)*poissonSample.y);
		//poissonSample += (shadowTexelStep * vec2(cos(randomAngle), sin(randomAngle)));

		poissonSample *= searchWidth;
		shadow += gaussian(length(poissonSample), (searchWidth/6.0f)*(searchWidth/6.0f))*
				cascadedShadowCalculation(viewDepth, worldPosition, constantShadowTexCoordDx, constantShadowTexCoordDy, bias, poissonSample);
	}
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


void main() {
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	vec4 sampledDepth = texture(gDepth, texCoord);
	float viewDepth = sampledDepth.r;
	vec3 finalColor = vec3(1,1,1);
	
	//point light shadow calculation
	
	for (int i = 0; i < numLights; i++) {
		float shadow = 1.0;
		if (containsShadow) {
			vec4 shadowTexCoord = vec4(position.xyz - lightPos[i].xyz, i);
			float depth = length(shadowTexCoord.xyz) / shadowFar;
			float dfdx = dFdx(depth);
			float dfdy = dFdy(depth);
			float depthSlope = sqrt(dfdx*dfdx + dfdy*dfdy);
			float bias = 0.01*depthSlope + 0.015;
			float diskRadius = .04f;
			diskRadius = clamp(diskRadius, 0.0f, MAX_POINT_LIGHT_DISK_RADIUS);
			int numSamples = int(mix(1, 81, pow( (diskRadius/MAX_POINT_LIGHT_DISK_RADIUS), 1.0f/2.2f) ));
			bias += 0.75*diskRadius;
			shadow = 1 - pointLightPoissonDisk(shadowMaps, shadowTexCoord, depth-bias, diskRadius, numSamples, position);
		}
		finalColor -= vec3(shadow);
	}
	
	
	if (containsShadow) {
		//cascaded shadow calculation
		float shadow = 1.0;
		float filterWidth = 5.0f;
		filterWidth = clamp(filterWidth, 0.0f, MAX_DIRECTIONAL_LIGHT_FILTER_WIDTH);
		//int numSamples = int(mix(1, 64, pow( (filterWidth/MAX_DIRECTIONAL_LIGHT_FILTER_WIDTH), 1.0f/3.f) ));
		int numSamples = 64;
		//finalColor -= (1 - vec3(cascadedShadowCalculationPoissonDisk(viewDepth, position, filterWidth, numSamples))); //widest is 12, shadow acne gets worse
		finalColor -= (1 - vec3(cascadedShadowCalculationPCF(viewDepth, position, 2)));
	}
	
	
	FragColor = vec4(finalColor, 1.0);
}