#pragma once
#include "util.h"

class Axis {
	public:
		Axis() {
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			const GLfloat vertices[] = {
				0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
			};
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			shader.loadFile("Shaders/Axis.vert", "Shaders/Axis.frag");
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat) ) );
		}
		void draw() {
			glBindVertexArray(vao);
			shader.useProgram();
			glDrawArrays(GL_LINES, 0, 6);
		}
		void setMat4(std::string name, const GLfloat* value) {
			shader.useProgram();
			shader.setMat4(name, value);
		}
		GLuint vao, vbo;
		Shader shader;
};