#pragma once
#include "util.h"
#include "Axis.h"
#include "Model.h"
#include "Renderer_constants.h"

extern Camera camera;
extern Camera cameraAlt;

//no instances actually made, just to define memory layout for offsetof()
struct MatricesUniformBlock {
	GLfloat view[4 * 4];
	GLfloat projection[4 * 4];
	GLfloat infiniteProj[4 * 4];
};

//padding made explicit
struct LightsUniformBlock {
	GLint numLights[4]; //int
	GLfloat cameraPos[4]; //vec3
	GLfloat lightPos[4 * maxPointLights];
	GLfloat lightColor[4 * maxPointLights];
	GLfloat lightParam[4 * maxPointLights]; //x: lightConstant, y: lightLinear, z: lightQuadratic, w: specularExponent
	GLfloat lightDir[4]; //vec3
	GLfloat lightDirColor[4];
};

struct ShadowUniformBlock {
	GLfloat shadowMatrices[4 * 4 * 6 * maxShadowedPointLights];
};

class ShadowCascadeTest {
public:
	ShadowCascadeTest() : vao{ 0 }, vbo{ 0 } {}
	void setupBuffer(int numShadowCascades);
	void fillBuffer(int numShadowCascades, glm::vec3* cascadedShadowBounds);
	void draw();
	GLuint vao, vbo;
	std::vector<GLuint> indices;
};

class Renderer {

public:
	Renderer(Renderer const&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer const&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	static Renderer* getInstance();

	void updateViewUniformBuffer(glm::mat4& viewMatrix);
	//debug scene used for previous versions of this code
	void setupTestScene();
	void renderTestScene();

	static glm::mat4 view;
	static glm::mat4 altView;
	static glm::mat4 projection;
	static glm::mat4 infiniteProj;

	static int numPointLights;
	static float lightDirTheta; //zenith angle, measured from x-y axis, where positive y axis points 90 degrees
	static float lightDirPhi; //azimuth angle, counterclockwise, where positive x axis points 0 degrees
	static glm::vec3 lightDir;
	static glm::vec4 lightDirColor;
	static std::vector<glm::vec4> lightPos;
	static std::vector<glm::vec4> lightColor;
	static std::vector<glm::vec4> lightParam;

	//displaying for debugging purposes
	enum RenderModes {
		RENDER_DEFAULT, RENDER_POSITION, RENDER_NORMAL, RENDER_ALBEDO, RENDER_DEPTH,
		RENDER_SPECULARITY, RENDER_SHADOW, RENDER_CASCADE_DEPTHS, RENDER_WIREFRAME, NUM_RENDER_MODES
	};
	enum DisplaySkybox { SKYBOX_DEFAULT, SKYBOX_SHADOW_MAP, NUM_SKYBOXES };
	enum RenderFrustumOutline { NO_FRUSTUM_OUTLINE, VIEW_FRUSTUM_OUTLINE, CASCADE_FRUSTUM_OUTLINE, NUM_FRUSTUM_OUTLINES };
	static int render_mode;
	static int skybox_mode;
	static int frustum_outline_mode;
	static int cameraMode;

private:

	Renderer() = default;
	~Renderer() = default;

	static Renderer* instance;

	enum UBONames { UBO_MATRICES, UBO_LIGHTS, UBO_SHADOW_MATRICES, NUM_UBO_NAMES };
	GLuint uboIDs[NUM_UBO_NAMES];
	void setupUniformBuffers();

	GLuint quadEBO = 0, quadVAO = 0, quadVBO = 0;
	void setupPlaneMesh();

	GLuint cubeVAO = 0, cubeVBO = 0;
	void setupCubeMesh();

	Shader skyboxShader;
	GLuint skyboxCubemapID = 0;
	GLint skyboxImageWidth = 0, skyboxImageHeight = 0, skyboxImageChannels = 0;
	void setupSkybox();

	Shader lightVolumeShader;
	void setupLightVolumes();

	GLuint cascadedShadowFramebufferID = 0;
	GLuint cascadedShadowTextureArrayID = 0;
	GLfloat shadowSplitDepths[numShadowCascades + 1];
	glm::vec3 cascadedShadowBounds[4 * (numShadowCascades + 1)]; //camera view space coordinates
	std::vector<GLfloat> cascadedShadowViewports;
	Shader cascadedShadowMapShader;
	void setupCascadedShadowMaps();

	Shader directionalAmbientShader;
	void setupDirectionalLightShader();

	GLuint shadowFramebufferID = 0;
	GLuint shadowCubemapArrayID = 0;
	Shader shadowMapShader;
	std::vector<glm::mat4> shadowMatrices;
	void setupPointShadowMaps();

	GLuint deferredFramebufferID = 0;
	GLint deferredFramebufferWidth = SCR_WIDTH, deferredFramebufferHeight = SCR_HEIGHT;
	std::vector<GLint> deferredAttachmentFormats{ GL_R32F, GL_RGBA16F, GL_RGBA16F, GL_RGBA16F };
	std::vector<GLuint> deferredAttachments; //0 = depth, 1 = position, 2 = normal, 3 = albedoSpec
	std::vector<GLuint> deferredColorTextureIDs;
	GLuint deferredDepthRenderbufferID = 0;
	Shader quadShader;
	void setupDeferredFramebuffer();

	Shader lightDisplayShader;
	void setupLightDisplay();

	Shader sunShader;
	void setupSunDisplay();

	void setupTestSceneLights();

	Shader shadowTestShader;
	ShadowCascadeTest shadowCascadeTest;
	Shader shadowCascadeTestShader;
	GLuint omniShadowMapViewID = 0;
	void setupShadowTesting();

	Axis testAxis;
	void setupTestAxis();

	Shader wireframeShader;
	void setupWireframeShader();

	GLint planeTextureWidth = 0, planeTextureHeight = 0, planeTextureChannels = 0;
	GLfloat planeScale = 50.0f;
	GLuint planeTextureID = 0;
	Shader planeShader;
	glm::mat4 quadModel;
	void setupTestScenePlane();

	Model testModel;
	Shader testModelShader;
	void setupTestSceneModel();

	void renderTestSceneDeferredPass();

	glm::mat4 cascadedShadowMatrices[numShadowCascades];
	glm::mat4 cameraViewToWorld = glm::inverse(view);
	glm::mat4 cascadedOrthos[numShadowCascades];
	glm::mat4 dirLightViews[numShadowCascades];
	glm::mat4 lightViewtoWorld = glm::mat4(1.0f);
	glm::vec3 tempShadowBounds[4 * (numShadowCascades + 1)];
	glm::vec3 cascadedShadowBoxBounds[8 * (numShadowCascades + 1)];
	void renderTestSceneShadowMapCascades();
	
	void renderTestScenePointShadowMaps();

	void renderTestSceneLightingPass();

	void drawFrustums();

	void drawSkybox();

	void drawMiscObjects();

	void fillShadowCascadeBuffer();
};