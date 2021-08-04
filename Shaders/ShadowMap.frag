#version 430 core

uniform float shadowFar = 25.0;
in vec3 worldPos;
flat in int lightNum;

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	vec4 lightParam[64]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
	vec3 lightDir;
	vec4 lightDirColor;
};

void main() {
	vec3 displacement = lightPos[lightNum].xyz - worldPos;
	gl_FragDepth = length(displacement) / shadowFar;
	
}