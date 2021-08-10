#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Renderer.h"

extern GLFWwindow* window;
extern const char* glsl_version;
extern bool windowHovered;

namespace UI {
	void initialize();
	void showDemoWindow();
	void renderUI();
	extern bool show_ui;
}