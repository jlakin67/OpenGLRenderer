#version 430 core

#define PI_OVER_TWO 1.570796f
#define ONE_OVER_PI 0.318310f
#define PHI 1.61803398874989484820459f

layout (location = 0) out float upperLeft;
layout (location = 1) out float upperRight;
layout (location = 2) out float lowerLeft;
layout (location = 3) out float lowerRight;
in vec2 texCoordIn;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
};

layout (std140, binding = 3) uniform Samples {
	vec2 poissonDisk[81]; //.1 disk radius, between 0 and 1
	vec3 poissonDiskSphere[81]; //.05 disk radius
	vec3 poissonDiskHemisphere[81]; //.025 disk radius
};

uniform sampler2D gDepth;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler3D noise;

uniform float radius = 0.5f;
uniform float bias = 0.05f;
uniform int numSamples = 25;

uniform int offset;

//upper left, upper right, lower left, lower right
//const vec2 texelStep[4] = {vec2(-1.0f, 1.0f), vec2(1.0f, 1.0f), vec2(-1.0f, -1.0f), vec2(1.0f, -1.0f)};
const ivec2 texelStep[4] = {ivec2(0,1), ivec2(1,1), ivec2(0,0), ivec2(1,0)};

uniform vec2 screenDim = vec2(800, 450);

void main() {
	//vec2 newTexCoord = texCoordIn + texelStep[gl_Layer]/textureSize(gDepth, 0);
	ivec2 newTexCoord = ivec2(2.0f*gl_FragCoord.xy) + texelStep[offset];
	vec4 worldPosition = vec4(texelFetch(gPosition, newTexCoord, 0).xyz, 1.0f);
	vec3 normal = normalize(texelFetch(gNormal, newTexCoord, 0).xyz); 
	vec2 randomAngles = texelFetch(noise, ivec3(offset, offset, offset), 0).xy;
	randomAngles.y -= PI_OVER_TWO; //sphere to hemisphere
	vec3 randomDir = vec3(cos(randomAngles.x)*sin(randomAngles.y),
						  sin(randomAngles.x)*sin(randomAngles.y),
						  cos(randomAngles.y));
	vec3 tangent = normalize(randomDir - dot(randomDir, normal) * normal);
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	float ao = 0.0f;
	for (int i = 0; i < numSamples; i++) {
		vec3 offset = TBN*poissonDiskHemisphere[i];
		//float scaleBias = mix(0.1f, 1.0f, (radius/2.5f)*(radius/2.5f));
		float sampleScale = mix(0.1f, 1.0f, (float(i)/float(numSamples))*(float(i)/float(numSamples)));
		vec3 sampleWorldPos = worldPosition.xyz + sampleScale*radius*offset;
		vec4 sampleViewPos = view*vec4(sampleWorldPos, 1.0f);
		vec4 sampleFragPos = projection*sampleViewPos;
		sampleFragPos /= sampleFragPos.w;
		sampleFragPos.xyz = 0.5f*sampleFragPos.xyz + vec3(0.5f);
		float projectedDepth = texture(gDepth, sampleFragPos.xy).r;
		vec3 projectedPosition = texture(gPosition, sampleFragPos.xy).xyz;
		float edgeScale = 1.0f - smoothstep(0.0f, 1.0f, length(projectedPosition - sampleWorldPos)/(radius));
		ao += edgeScale*(-sampleViewPos.z - bias >= projectedDepth ? 1.0f : 0.0f);
	}
	float aoOut = 1.0f - (ao/float(numSamples));
	upperLeft = aoOut * (offset == 0 ? 1 : 0);
	upperRight = aoOut* (offset == 1 ? 1 : 0);
	lowerLeft = aoOut* (offset == 2 ? 1 : 0);
	lowerRight = aoOut* (offset == 3 ? 1 : 0);
	//FragColor = dot(texelStep[offset], vec2(1,1)) / 2.0f;
}