#version 430 core

//used with light volumes, ambient with a directional light

out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform sampler2D gAlbedoSpec;
uniform bool containsShadow = false;
uniform float ambientStrength = 0.3;
uniform float shadowFar = 25.0;
uniform int numShadowCascades = 3;
uniform float shadowSplitDepths[6];
uniform sampler2DArrayShadow shadowCascades;
uniform mat4 cascadedShadowMatrices[5];

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	vec4 lightParam[64]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
	vec3 lightDir;
	vec4 lightDirColor; //to disable set lightDirColor to vec3(0,0,0)
};

vec3 dirLightShading(vec4 diffuseColor, float specularStrength, vec3 normal, vec3 position) {
	vec3 finalColor = vec3(0.0,0.0,0.0);
	float diffuseStrength = max(dot(lightDir, normal), 0);
	finalColor += diffuseStrength * lightDirColor.rgb * diffuseColor.rgb;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(cameraPos - position);
	float spec = pow(max(dot(normal, reflectDir), 0), 16.0f);
	finalColor += specularStrength * spec * lightDirColor.rgb;
	return finalColor;
}

float cascadedShadowCalculation(float viewDepth, vec4 worldPosition) {
	float shadow = 1.0;
	int shadowCascadeIndex = 0;
	for (int i = 0; i < numShadowCascades; i++) {
		if (viewDepth > shadowSplitDepths[i] && viewDepth <= shadowSplitDepths[i+1]) {
			shadowCascadeIndex = i;
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
	float bias = 0.05*depthSlope + 0.001;
	//bias /= (shadowCascadeIndex + 4.0f);
	shadowTextureCoordinate.z -= bias;
	shadowTextureCoordinate.xyzw = shadowTextureCoordinate.xywz; //last component is depth reference for comparison
	//second to last is the index into the texture array
	shadowTextureCoordinate.z = shadowCascadeIndex;
	shadow = texture(shadowCascades, shadowTextureCoordinate);
	return shadow;
}

in vec2 texCoord;

void main() {
	vec4 normal = texture(gNormal, texCoord);
	if (normal.a == 0) discard;
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	vec4 albedoSpec = texture(gAlbedoSpec, texCoord);
	vec4 diffuse = vec4(albedoSpec.rgb, 1.0);
	vec4 sampledDepth = texture(gDepth, texCoord);
	float viewDepth = sampledDepth.r;
	float specularStrength = albedoSpec.a;
	vec3 finalColor = vec3(0,0,0);
	float shadow = 1.0;
	if (containsShadow) {
		shadow = cascadedShadowCalculation(viewDepth, position);
	}
	finalColor += shadow*dirLightShading(diffuse, specularStrength, normal.xyz, position.xyz);
	finalColor += ambientStrength*diffuse.rgb;
	FragColor = vec4(finalColor,1.0);
}