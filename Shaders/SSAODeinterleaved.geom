#version 430 core
layout (triangles, invocations = 4) in;
layout (triangle_strip, max_vertices = 24) out;

in vec2 texCoord[];
out vec2 texCoordIn;
flat out int layer;

void main() { 
	for (int i = 0; i < gl_in.length; i++) {
		gl_Position = gl_in[i].gl_Position;
		texCoordIn = texCoord[i];
		gl_Layer = gl_InvocationID;
		layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}