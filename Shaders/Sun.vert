#version 430 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
};

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
	mat4 model = mat4(1.0);
	model[3] = vec4(45*lightDir, 1.0);
	mat4 newView = view;
	newView[3] = vec4(0,0,0,1);
	vec4 pos = projection*newView*model*vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}