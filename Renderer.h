#pragma once
#include "util.h"
#include "Axis.h"
#include "Model.h"
#include "Renderer_constants.h"
#include <chrono>
#include <random>

//setup general model asset shader with different shading modes controlled by uniform 
//and ui for transforming and adding model assets

extern Camera camera;
extern Camera cameraAlt;

//no instances actually made, just to define memory layout for offsetof()
struct MatricesUniformBlock {
	GLfloat view[4 * 4];
	GLfloat projection[4 * 4];
	GLfloat infiniteProj[4 * 4];
};

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

struct SamplesBlock {
	GLfloat poissonDisk[4 * maxSamples]; //vec2[81]
	GLfloat poissonDiskSphere[4 * maxSamples]; //vec3[81]
	GLfloat poissonDiskHemisphere[4 * maxSamples]; //vec3[81]
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
	void updatePointLight(int index, glm::vec4* position, glm::vec4* color, glm::vec4* param);
	void updateDirectionalLight(glm::vec4* newLightDir, glm::vec4* newLightDirColor);

	void setupTestScene(); //old, debug scene used for previous versions of this code
	void setup();
	void renderTestScene(); //old
	void render();

	static glm::mat4 view;
	static glm::mat4 altView;
	static glm::mat4 projection;
	static glm::mat4 infiniteProj;

	static int numPointLights;
	static glm::vec3 lightDir;
	static glm::vec4 lightDirColor;
	static std::vector<glm::vec4> lightPos;
	static std::vector<glm::vec4> lightColor;
	static std::vector<glm::vec4> lightParam;
	static std::vector<Model*> modelAssets;

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
	~Renderer() { 
		if (instance) delete instance;
		for (Model* model : modelAssets) {
			delete model;
		}
	}

	static Renderer* instance;

	enum UBONames { UBO_MATRICES, UBO_LIGHTS, UBO_SHADOW_MATRICES, UBO_SAMPLES, NUM_UBO_NAMES };
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
	GLuint lightVolumeModelBufID = 0;
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

	GLuint randomTextureID = 0;
	void setupRandomTexture();

	GLuint deferredFramebufferID = 0;
	GLint deferredFramebufferWidth = SCR_WIDTH, deferredFramebufferHeight = SCR_HEIGHT;
	std::vector<GLint> deferredAttachmentFormats{ GL_R32F, GL_RGBA16F, GL_RGBA16F, GL_RGBA16F, GL_RGBA16F};
	std::vector<GLuint> deferredAttachments; //0 = depth, 1 = position, 2 = normal, 3 = albedo, 4 = specular/exponent
	std::vector<GLuint> deferredColorTextureIDs;
	GLuint deferredDepthRenderbufferID = 0;
	Shader quadShader;
	void setupDeferredFramebuffer();

	//gamma correction, tone mapping, anti-aliasing
	GLuint postprocessFramebufferID = 0;
	GLuint postprocessColorTextureID = 0;
	GLuint postprocessRenderbufferID = 0;
	Shader postprocessShader;
	void setupPostprocessFramebuffer();

	Shader lightDisplayShader;
	GLuint lightDisplayPositionBufID = 0;
	void setupLightDisplay();

	Shader sunShader;
	void setupSunDisplay();

	void setupTestSceneLights(); //old

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
	void setupTestScenePlane(); //old

	
	Shader modelShader;
	void setupModelShader();

	void setupTestSceneModel(); //old

	void renderTestSceneDeferredPass(); //old
	void renderDeferredPass();

	glm::mat4 cascadedShadowMatrices[numShadowCascades];
	glm::mat4 cameraViewToWorld = glm::inverse(view);
	glm::mat4 lightViewtoWorld = glm::mat4(1.0f);
	glm::vec3 worldShadowBounds[4 * (numShadowCascades + 1)];
	glm::vec3 cascadedShadowBoxBounds[8 * (numShadowCascades + 1)];
	void renderTestSceneShadowMapCascades(); //old
	void renderShadowMapCascades();
	
	void renderTestScenePointShadowMaps(); //old
	void renderPointShadowMaps();

	void renderLightingPass();

	void drawFrustums();

	void drawSkybox();

	void drawMiscObjects();

	void fillShadowCascadeBuffer();
};