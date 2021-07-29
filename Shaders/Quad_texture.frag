#version 430 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 pos;
uniform sampler2D tex;

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	float specularExponent;
	float lightLinear;
	float lightQuadratic;
};

vec3 pointLightShading(vec3 lightPos, vec4 lightColor, vec4 diffuseColor) {
	vec3 newNormal = normal;
	if (!gl_FrontFacing) newNormal *= -1;
	vec3 lightDir = normalize(lightPos - pos);
	float dotProduct = dot(lightDir, newNormal);
	if (dotProduct < 0) {
		return vec3(0,0,0);
	}
	else {
		float distance = length(lightPos - pos);
		float attenuation = 1 / (1.0 + (lightLinear*distance) + (lightQuadratic*pow(distance, 2.0)));
		float diffuseStrength = max(dotProduct, 0);
		vec3 diffuse = diffuseStrength*lightColor.rgb*diffuseColor.rgb;
		vec3 viewDir = normalize(cameraPos - pos);
		vec3 halfwayDir = normalize(viewDir+lightDir);
		vec3 specular;
		float spec = pow(max(dot(newNormal, halfwayDir), 0), specularExponent);
		specular = spec*vec3(lightColor);
		return attenuation*(diffuse+specular);
	}
}

void main() {
	vec3 finalColor = vec3(0,0,0);
	vec4 diffuseColor = texture(tex, texCoord);
	if (diffuseColor.a < 0.01) discard;
	for (int i = 0; i < numLights; i++) {
		finalColor += pointLightShading(vec3(lightPos[i]), lightColor[i], diffuseColor);
	}
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength*vec3(diffuseColor);
	finalColor += ambient;
	FragColor = vec4(finalColor, 1.0);
}