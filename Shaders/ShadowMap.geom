#version 430 core
//maxShadowedLights = 30
layout (triangles, invocations = 6) in;
layout (triangle_strip, max_vertices = 90) out; //30*3

layout (std140, binding = 1) uniform Lights {
	int numLights;
	vec3 cameraPos;
	vec4 lightPos[64];
	vec4 lightColor[64];
	vec4 lightParam[64]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
	vec3 lightDir;
	vec4 lightDirColor;
};

layout (std140, binding = 2) uniform ShadowMatrices {
	mat4 shadowMatrices[6*30];
};

in VS_GS_Interface {
	vec2 texCoord;
} gs_in[];

uniform int numShadowedLights;

out GS_FS_Interface {
    vec3 worldPos;
    vec2 texCoord;
    flat int lightNum;
} gs_out;


void main() {
	for (int i = 0; i < numShadowedLights; i++) {
		for (int j = 0; j < 3; j++) {
			gl_Layer = 6*i + gl_InvocationID;
			gs_out.worldPos = gl_in[j].gl_Position.xyz;
			gs_out.texCoord = gs_in[j].texCoord;
			gl_Position = shadowMatrices[6*i + gl_InvocationID]*gl_in[j].gl_Position;
			gs_out.lightNum = i;
			EmitVertex();
		}
		EndPrimitive();
	}
}