#version 430 core

layout (triangles) in;

//maxShadowCascades = 5
layout (triangle_strip, max_vertices = 15) out;

uniform int numShadowCascades = 3;
uniform mat4 shadowMatrices[5];

void main() {
	for (int i = 0; i < numShadowCascades; i++) {
		for (int j = 0; j < gl_in.length; j++) { 
			gl_Position = shadowMatrices[i]*gl_in[j].gl_Position;
			gl_ViewportIndex = i;
			gl_Layer = i;
			EmitVertex();
		}		
		EndPrimitive();
	}
}