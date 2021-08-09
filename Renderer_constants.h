#pragma once
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include <algorithm>

constexpr float pi = 3.14159265f;
constexpr int maxPointLights = 64;
constexpr int maxShadowedPointLights = 30;
constexpr GLsizei numShadowCascades = 4;
constexpr GLuint shadowWidth = 1024 * 2;
constexpr GLuint shadowHeight = 1024 * 2;
constexpr GLuint directionalShadowWidth = 1024 * 4;
constexpr GLuint directionalShadowHeight = 1024 * 4;
constexpr GLfloat shadow_aspect = 1.0f;
constexpr GLfloat shadow_near = 1.0f;
constexpr GLfloat shadow_far = 30.0f;
constexpr GLint shadowFilter = GL_LINEAR;
constexpr GLfloat shadowSplitLinearFactor = 0.6f;
const GLsizei numShadowMipmaps = std::max(static_cast<GLsizei>(std::log2(shadowWidth)), 1);
constexpr GLfloat near = 0.1f;
constexpr GLfloat far = 100.0f;
constexpr GLfloat maxDepth = 1.0f;
constexpr GLuint SCR_WIDTH = 1600;
constexpr GLuint SCR_HEIGHT = 900;
const float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
const float top = near * std::tanf(glm::radians(ZOOM));
const float bottom = -top;
const float right = top * aspectRatio;
const float left = -right;
constexpr GLfloat ambientStrength = 0.01f;
constexpr GLfloat specularExponent = 16.0f;
constexpr GLfloat lightLinear = 0.09f;
constexpr GLfloat lightQuadratic = 0.032f;
constexpr GLfloat lightConstant = 1.0f;
constexpr GLfloat maxAnisotropy = 16.0f;
extern const GLfloat cube_vertices[108];
extern const GLfloat quadNormal[3];
constexpr GLint randomTextureSize = 64;
extern GLfloat randomAngleTexture[randomTextureSize * randomTextureSize * randomTextureSize];
constexpr int maxSamples = 81;
extern GLfloat poissonDisk[4 * maxSamples];
extern GLfloat poissonDiskDir[4 * maxSamples];
extern GLfloat poissonDiskDirHemi[4 * maxSamples];