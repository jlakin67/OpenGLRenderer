#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "UI.h"
#include "util.h"
#include <iostream>

const char* glsl_version = "#version 430";
double currentFrame = 0.0f, lastFrame = 0.0f, deltaTime = 0.0f;
Camera camera(glm::vec3(0.0f, 1.0f, 0.0f));
Camera cameraAlt(glm::vec3(0.0f, 1.0f, 0.0f));
const GLfloat maxAnisotropy = 16.0f;
GLFWwindow* window = NULL;

int main(int argc, char* argv[])
{
    bool useDebug = true;
    if (argc == 2) {
        if (strcmp(argv[1], "nd") == 0) useDebug = false;
    }
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (useDebug) glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL 4.3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    if (useDebug) {
        GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
        else {
            std::cout << "Failed to create debug context\n";
            glfwTerminate();
            return -1;
        }
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1); //v-sync
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    printf("Vendor:          %s\n", glGetString(GL_VENDOR));
    printf("Renderer:        %s\n", glGetString(GL_RENDERER));
    printf("Version OpenGL:  %s\n", glGetString(GL_VERSION));
    printf("Version GLSL:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    //initialize imgui
    UI::initialize();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glfwSetTime(0);
    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, camera);

        //Imgui frame
        UI::renderUI();
        
        glfwSwapBuffers(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}