#include "util.h"

bool firstMove = true, mousePressed = false;
double lastX = 0, lastY = 0, deltaX = 0, deltaY = 0;
extern Camera camera;
extern Camera cameraAlt;
GLuint SCR_WIDTH = 1600;
GLuint SCR_HEIGHT = 900;
GLint renderMode = 0;
GLint matrixMode = 2;
int renderCascadeFrustum = 0;
bool space_key_pressed = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

void processInput(GLFWwindow* window, Camera& camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.processKeyboard(GLFW_KEY_W, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_S, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_D, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_A, deltaTime);
    }
    if (!space_key_pressed) space_key_pressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && space_key_pressed) { //toggle UI
        UI::show_ui = !UI::show_ui;
        space_key_pressed = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    bool uiHovered = UI::isWindowHovered();
    if (firstMove) {
        lastX = xpos;
        lastY = ypos;
        firstMove = false;
    }
    else {
        deltaX = xpos - lastX;
        deltaY = ypos - lastY;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        deltaX = 0;
        deltaY = 0;
    }
    lastX = xpos; lastY = ypos;
    camera.processMouse(window, deltaX, deltaY, uiHovered);
}

void drawLights(std::vector<glm::vec4>& positions, std::vector<glm::vec4>& colors, 
    Generic& generic, Shader& shader) {
    shader.useProgram();
    for (int i = 0; i < positions.size(); i++) {
        glm::vec4 position = positions.at(i);
        glm::mat4 new_model = generic.model;
        new_model[3] = position;
        shader.setMat4("model", glm::value_ptr(new_model));
        shader.setVec4("color", glm::value_ptr(colors.at(i)));
        generic.draw();
    }
}

void drawLights(std::vector<glm::vec4>& positions, std::vector<glm::vec4>& colors, Model& model,
    Shader& shader) {
    shader.useProgram();
    for (int i = 0; i < positions.size(); i++) {
        glm::vec4 position = positions.at(i);
        glm::mat4 new_model = model.model;
        new_model[3] = position;
        model.model = new_model;
        model.draw(shader, colors.at(i));
    }
}

float depthSplitFuncLinear(float near, float far, int numShadowCascades, int i)
{
    return near + (static_cast<GLfloat>(i) / static_cast<GLfloat>(numShadowCascades)) * (far - near);
}

float customDepthSplitFunc(float near, float far, int numShadowCascades, int i)
{
    if (i == 0) return near;
    else if (i == 1) return near + 0.5f * (far - near);
    else return depthSplitFuncLinear(near + 0.5f * (far - near), far, numShadowCascades, i);
}

void ShadowCascadeTest::setupBuffer(int numShadowCascades)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint ebo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(4) * 
        (static_cast<GLsizeiptr>(numShadowCascades) + 1) * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //0 upper left
    //1 upper right
    //2 bottom left
    //3 bottom right
    for (int i = 0; i < 4 * (numShadowCascades); i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 1);
        indices.push_back(i + 3);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i);
        indices.push_back(i);
        indices.push_back(i + 4);
        indices.push_back(i + 1);
        indices.push_back(i + 1 + 4);
        indices.push_back(i + 2);
        indices.push_back(i + 2 + 4);
        indices.push_back(i + 3);
        indices.push_back(i + 3 + 4);
    }
    indices.push_back(4*numShadowCascades);
    indices.push_back(4 * numShadowCascades + 1);
    indices.push_back(4 * numShadowCascades + 1);
    indices.push_back(4 * numShadowCascades + 3);
    indices.push_back(4 * numShadowCascades + 3);
    indices.push_back(4 * numShadowCascades + 2);
    indices.push_back(4 * numShadowCascades + 2);
    indices.push_back(4 * numShadowCascades);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void ShadowCascadeTest::fillBuffer(int numShadowCascades, glm::vec3* cascadedShadowBounds) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(4) *
        (static_cast<GLsizeiptr>(numShadowCascades) + 1) * sizeof(glm::vec3), cascadedShadowBounds);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ShadowCascadeTest::draw()
{
    glBindVertexArray(vao);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
