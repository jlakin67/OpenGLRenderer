#include "Renderer.h"

namespace Renderer {
    void initializeUniformBuffer(GLuint bp) {

    }
    namespace {
        //private stuff
    }
}


//initialization of global variables related to rendering

const float Renderer::pi = 3.14159265f;

const GLfloat Renderer::near = 0.1f;
const GLfloat Renderer::far = 100.0f;
const float Renderer::aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
const float Renderer::top = Renderer::near * std::tanf(glm::radians(ZOOM));
const float Renderer::bottom = -Renderer::top;
const float Renderer::right = Renderer::top * Renderer::aspectRatio;
const float Renderer::left = -Renderer::right;
const GLfloat Renderer::maxDepth = 1.0f;
glm::mat4 Renderer::projection = glm::perspective(glm::radians(ZOOM), Renderer::aspectRatio, Renderer::near, Renderer::far);
glm::mat4 Renderer::view = glm::lookAt(camera.pos, camera.pos + camera.front, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 Renderer::altView = glm::lookAt(cameraAlt.pos, cameraAlt.pos + cameraAlt.front, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 Renderer::infiniteProj = glm::infinitePerspective(glm::radians(ZOOM), Renderer::aspectRatio, Renderer::near);
Renderer::MatricesUniformBlock Renderer::matrices;

const GLfloat Renderer::maxAnisotropy = 16.0f;

int Renderer::render_mode = Renderer::RENDER_DEFAULT;
int Renderer::skybox_mode = Renderer::SKYBOX_DEFAULT;
int Renderer::frustum_outline_mode = Renderer::NO_FRUSTUM_OUTLINE;

int Renderer::numPointLights = 2;
const GLfloat Renderer::specularExponent = 16.0f;
const GLfloat Renderer::lightLinear = 0.09f;
const GLfloat Renderer::lightQuadratic = 0.032f;
const GLfloat Renderer::lightConstant = 1.0f;
float Renderer::lightDirTheta = glm::radians(45.0f); //zenith angle, measured from x-y axis, where positive y axis points 90 degrees
float Renderer::lightDirPhi = glm::radians(-90.0f); //azimuth angle, counterclockwise, where positive x axis points 0 degrees
glm::vec3 Renderer::lightDir(cos(Renderer::lightDirTheta)* cos(Renderer::lightDirPhi),
	sin(Renderer::lightDirTheta),
	-sin(Renderer::lightDirPhi) * cos(Renderer::lightDirTheta)
);
glm::vec4 Renderer::lightDirColor(0.7f, 0.7f, 0.7f, 1.0f);
Renderer::LightsUniformBlock Renderer::lights;
const GLfloat Renderer::ambientStrength = 0.01f;
const GLuint Renderer::shadowWidth = 1024 * 2;
const GLuint Renderer::shadowHeight = 1024 * 2;
const GLuint Renderer::directionalShadowWidth = 1024;
const GLuint Renderer::directionalShadowHeight = 1024;
GLfloat Renderer::shadow_aspect = (GLfloat)shadowWidth / (GLfloat)shadowHeight;
GLfloat Renderer::shadow_near = 1.0f;
GLfloat Renderer::shadow_far = 30.0f;
const GLsizei Renderer::numShadowMipmaps = std::max(static_cast<GLsizei>(std::log2(Renderer::shadowWidth)), 1);
const GLsizei Renderer::numShadowCascades = 4;
const GLint Renderer::shadowFilter = GL_LINEAR;
const GLfloat Renderer::shadowSplitLinearFactor = 0.6f;

const GLfloat Renderer::cube_vertices[108]{
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
};