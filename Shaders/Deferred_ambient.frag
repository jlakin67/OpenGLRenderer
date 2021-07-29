#version 430 core

//with light volumes, ambient only

out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform bool containsShadow = false;
uniform float ambientStrength = 0.3;
uniform float far = 25.0;

in vec2 texCoord;

void main() {
	vec4 normal = texture(gNormal, texCoord);
	if (normal.a == 0) discard;
	vec4 position = texture(gPosition, texCoord);
	if (position.a == 0) discard;
	vec4 albedoSpec = texture(gAlbedoSpec, texCoord);
	vec4 diffuse = vec4(albedoSpec.rgb, 1.0);
	vec3 finalColor = vec3(0,0,0);
	finalColor.rgb += ambientStrength*diffuse.rgb;
	FragColor = vec4(finalColor,1.0);
}