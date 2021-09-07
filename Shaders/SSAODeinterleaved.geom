#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 24) out;

in vec2 texCoord[];
out vec2 texCoordIn;
flat out int layer;

void main() {
	for (int i = 0; i < 4; i++) { 
		for (int j = 0; j < gl_in.length; j++) {
			gl_Position = gl_in[j].gl_Position;
			texCoordIn = texCoord[j];
			gl_Layer = i;
			layer = i;
			EmitVertex();
		}
		EndPrimitive();
	}
}