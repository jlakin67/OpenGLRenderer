#version 430 core

//with light volumes, blinn phong

out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform bool containsShadow = false;
uniform samplerCubeArrayShadow shadowMaps;
uniform float ambientStrength = 0.3;
uniform float shadowFar = 25.0;

uniform vec2 windowSize = vec2(1600, 900);
flat in int instanceID;
flat in mat4 light_model;

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

float isShadow(samplerCubeArrayShadow shadowMaps, vec4 texCoord, float depth) {
	float shadow = 1.0;
	if (depth > 1.0) return shadow;
	shadow = texture(shadowMaps, texCoord, depth);
	return shadow;
}

void main() {
	vec2 texCoord = vec2(gl_FragCoord.x/windowSize.x, gl_FragCoord.y/windowSize.y);
	
	vec4 normal = texture(gNormal, texCoord);
	if (normal.a == 0) discard;
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	if (length(position - light_model[3]) > light_model[0][0]) discard;
	vec4 albedoSpec = texture(gAlbedoSpec, texCoord);
	vec4 diffuse = vec4(albedoSpec.rgb, 1.0);
	vec3 finalColor = vec3(0,0,0);
	float shadow = 1.0;
	if (containsShadow) {
		vec4 shadowTexCoord = vec4(position.xyz - lightPos[instanceID].xyz, instanceID);
			float depth = length(shadowTexCoord.xyz) / shadowFar;
			float dfdx = dFdx(depth);
			float dfdy = dFdy(depth);
			float depthSlope = sqrt(dfdx*dfdx + dfdy*dfdy);
			float bias = 0.01*depthSlope + 0.015;
		shadow = isShadow(shadowMaps, shadowTexCoord, depth-bias);
	}
	vec4 param = lightParam[instanceID];
	finalColor += shadow*pointLightShading(lightPos[instanceID].xyz, lightColor[instanceID], diffuse, 
	normal.xyz, albedoSpec.a, position.xyz, param);
	
	FragColor = vec4(finalColor, 1.0);
}