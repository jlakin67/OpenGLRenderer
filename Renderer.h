#pragma once
#include "util.h"
#include "Axis.h"
#include "UBO.h"

extern Camera camera;
extern Camera cameraAlt;
extern GLuint SCR_WIDTH;
extern GLuint SCR_HEIGHT;

//contains public variables and functions
namespace Renderer {
	
	void updateViewUniformBuffer(glm::mat4& viewMatrix);
	//debug scene used for previous versions of this code
	void setupTestScene();
	void renderTestScene();

	//screen projection
	extern const float pi;
	extern const GLfloat near;
	extern const GLfloat far;
	extern const float aspectRatio;
	extern const float top;
	extern const float bottom;
	extern const float right;
	extern const float left;
	extern const GLfloat maxDepth;
	extern glm::mat4 view;
	extern glm::mat4 altView;
	extern glm::mat4 projection;
	extern glm::mat4 infiniteProj;
	struct MatricesUniformBlock {
		GLfloat view[4*4];
		GLfloat projection[4*4];
		GLfloat infiniteProj[4*4];
	};

	extern const GLfloat maxAnisotropy;

	//light and shadow
	constexpr int maxPointLights = 64;
	constexpr int maxShadowedPointLights = 30;
	extern int numPointLights;
	extern const GLfloat specularExponent;
	extern const GLfloat lightLinear;
	extern const GLfloat lightQuadratic;
	extern const GLfloat lightConstant;
	extern float lightDirTheta; //zenith angle, measured from x-y axis, where positive y axis points 90 degrees
	extern float lightDirPhi; //azimuth angle, counterclockwise, where positive x axis points 0 degrees
	extern glm::vec3 lightDir;
	extern glm::vec4 lightDirColor;
	//padding made explicit
	struct LightsUniformBlock {
		GLint numLights[4]; //int
		GLfloat cameraPos[4]; //vec3
		GLfloat lightPos[4*maxPointLights];
		GLfloat lightColor[4*maxPointLights];
		GLfloat lightParam[4*maxPointLights]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
		GLfloat lightDir[4]; //vec3
		GLfloat lightDirColor[4];
	};
	extern std::vector<glm::vec4> lightPos;
	extern std::vector<glm::vec4> lightColor;
	extern std::vector<glm::vec4> lightParam;
	extern const GLfloat ambientStrength;
	extern const GLuint shadowWidth;
	extern const GLuint shadowHeight;
	extern const GLuint directionalShadowWidth;
	extern const GLuint directionalShadowHeight;
	extern GLfloat shadow_aspect;
	extern GLfloat shadow_near;
	extern GLfloat shadow_far;
	extern const GLsizei numShadowMipmaps;
	constexpr GLsizei numShadowCascades = 4;
	extern const GLint shadowFilter;
	extern const GLfloat shadowSplitLinearFactor;
	struct ShadowUniformBlock {
		GLfloat shadowMatrices[4 * 4 * maxShadowedPointLights];
	};

	//displaying for debugging purposes
	enum RenderModes {RENDER_DEFAULT, RENDER_POSITION, RENDER_NORMAL, RENDER_ALBEDO, RENDER_DEPTH,
					  RENDER_SPECULARITY, RENDER_SHADOW, RENDER_CASCADE_DEPTHS, RENDER_WIREFRAME, NUM_RENDER_MODES};
	enum DisplaySkybox {SKYBOX_DEFAULT, SKYBOX_SHADOW_MAP, NUM_SKYBOXES};
	enum RenderFrustumOutline {NO_FRUSTUM_OUTLINE, VIEW_FRUSTUM_OUTLINE, CASCADE_FRUSTUM_OUTLINE, NUM_FRUSTUM_OUTLINES};
	extern int render_mode;
	extern int skybox_mode;
	extern int frustum_outline_mode;

	extern const GLfloat cube_vertices[108];
}

class ShadowCascadeTest {
public:
	ShadowCascadeTest() : vao{ 0 }, vbo{ 0 } {}
	void setupBuffer(int numShadowCascades);
	void fillBuffer(int numShadowCascades, glm::vec3* cascadedShadowBounds);
	void draw();
	GLuint vao, vbo;
	std::vector<GLuint> indices;
};