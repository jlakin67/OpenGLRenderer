#version 430 core

uniform float shadowFar = 25.0;
uniform bool containsDiffuse = false;
uniform sampler2D texture_diffuse0;

in GS_FS_Interface {
    vec3 worldPos;
    vec2 texCoord;
    flat int lightNum;
} fs_in;

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
	if (containsDiffuse) {
		if (texture(texture_diffuse0, fs_in.texCoord).a < 0.01) discard;
	}
	vec3 displacement = lightPos[fs_in.lightNum].xyz - fs_in.worldPos;
	gl_FragDepth = length(displacement) / shadowFar;
	
}