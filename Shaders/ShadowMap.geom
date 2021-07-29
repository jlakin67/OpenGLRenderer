#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrix[6];
out vec3 worldPos;

void main() {
	for (int i = 0; i < 6; i++) {
		gl_Layer = i;
		for (int j = 0; j < 3; j++) {
			worldPos = gl_in[j].gl_Position.xyz;
			gl_Position = shadowMatrix[i]*gl_in[j].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}