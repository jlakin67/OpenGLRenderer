#pragma once
#include "util.h"

extern Camera camera;
extern Camera cameraAlt;
extern GLuint SCR_WIDTH;
extern GLuint SCR_HEIGHT;

//contains everything to do with rendering
namespace Renderer {

	void initializeUniformBuffer(GLuint bp);

	extern const float pi;
	extern const GLfloat near;
	extern const GLfloat far;
	extern const float aspectRatio;
	extern const float top;
	extern const float bottom;
	extern const float right;
	extern const float left;
	extern const GLfloat maxDepth;
	extern glm::mat4 projection;
	extern glm::mat4 view;
	extern glm::mat4 altView;
	extern glm::mat4 infiniteProj;
	struct MatricesUniformBlock {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 infiniteProjection;
	};
	extern MatricesUniformBlock matrices;

	extern const GLfloat maxAnisotropy;

	constexpr int maxPointLights = 64;
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
		glm::ivec4 numLights; //int
		glm::vec4 cameraPos; //vec3
		glm::vec4 lightPos[maxPointLights];
		glm::vec4 lightColor[maxPointLights];
		glm::vec4 lightParam[maxPointLights]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
		glm::vec4 lightDir; //vec3
		glm::vec4 lightDirColor;
	};
	extern LightsUniformBlock lights;
	extern const GLfloat ambientStrength;
	extern const GLuint shadowWidth;
	extern const GLuint shadowHeight;
	extern const GLuint directionalShadowWidth;
	extern const GLuint directionalShadowHeight;
	extern GLfloat shadow_aspect;
	extern GLfloat shadow_near;
	extern GLfloat shadow_far;
	extern const GLsizei numShadowMipmaps;
	extern const GLsizei numShadowCascades;
	extern const GLint shadowFilter;
	extern const GLfloat shadowSplitLinearFactor;

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