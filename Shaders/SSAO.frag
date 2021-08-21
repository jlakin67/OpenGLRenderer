#version 430 core

#define PI_OVER_TWO 1.570796f
#define ONE_OVER_PI 0.318310f
#define PHI 1.61803398874989484820459f

out float FragColor;
in vec2 texCoord;

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

void main() {
	vec4 worldPosition = vec4(texture(gPosition, texCoord).xyz, 1.0f);
	vec3 normal = normalize(texture(gNormal, texCoord).xyz); 
	vec2 randomAngles = texture(noise, 32.0f*sin(100.0f*worldPosition.xyz) + 32.0f).xy;
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
	FragColor = 1.0f - (ao/float(numSamples));
	//FragColor = pow(FragColor, 2.2);
}