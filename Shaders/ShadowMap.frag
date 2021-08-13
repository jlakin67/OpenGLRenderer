#version 430 core

uniform float shadowFar = 25.0;
uniform bool containsDiffuse = false;
uniform sampler2D texture_diffuse0;
uniform bool containsAlpha = false;
uniform sampler2D texture_alpha;
uniform vec4 color = vec4(0.2f, 0.2f, 0.2f, 1.0f);

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
	float alpha;
	if (containsDiffuse) {
		alpha = texture(texture_diffuse0, fs_in.texCoord).a;
	} else alpha = color.a;
	if (containsAlpha) {
		alpha = texture(texture_alpha, fs_in.texCoord).r;
	}
	if (alpha < 0.01f) discard;
	vec3 displacement = lightPos[fs_in.lightNum].xyz - fs_in.worldPos;
	gl_FragDepth = length(displacement) / shadowFar;
	
}