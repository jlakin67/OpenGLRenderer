#include "Renderer.h"

void Renderer::setupUniformBuffers() {
	glGenBuffers(NUM_UBO_NAMES, uboIDs);
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_MATRICES]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MatricesUniformBlock), NULL, GL_DYNAMIC_DRAW);
	projection = glm::perspective(glm::radians(ZOOM), aspectRatio, near, far);
	infiniteProj = glm::infinitePerspective(glm::radians(ZOOM), aspectRatio, near);
	view = glm::lookAt(camera.pos, camera.pos + camera.front, glm::vec3(0.0f, 1.0f, 0.0f));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(MatricesUniformBlock, view),
		sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(MatricesUniformBlock, projection),
		sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(MatricesUniformBlock, infiniteProj),
		sizeof(glm::mat4), glm::value_ptr(infiniteProj));
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboIDs[UBO_MATRICES]);
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_LIGHTS]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightsUniformBlock), NULL, GL_DYNAMIC_DRAW);
	glm::ivec4 paddedNumLights(numPointLights, 0, 0, 0);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, numLights),
		sizeof(glm::ivec4), glm::value_ptr(paddedNumLights));
	glm::vec4 paddedCameraPos = glm::vec4(camera.pos, 1.0f);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, cameraPos),
		sizeof(glm::vec4), glm::value_ptr(paddedCameraPos));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightPos),
		numPointLights * sizeof(glm::vec4), glm::value_ptr(lightPos[0]));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightColor),
		numPointLights * sizeof(glm::vec4), glm::value_ptr(lightColor[0]));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightParam),
		numPointLights * sizeof(glm::vec4), glm::value_ptr(lightParam[0]));
	glm::vec4 paddedLightDir(lightDir, 0.0f);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightDir),
		sizeof(glm::vec4), glm::value_ptr(paddedLightDir));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightDirColor),
		sizeof(glm::vec4), glm::value_ptr(lightDirColor));
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboIDs[UBO_LIGHTS]);
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_SHADOW_MATRICES]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ShadowUniformBlock), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboIDs[UBO_SHADOW_MATRICES]);
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_SAMPLES]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SamplesBlock), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SamplesBlock, poissonDisk), sizeof(poissonDisk), poissonDisk);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SamplesBlock, poissonDiskSphere), sizeof(poissonDiskDir), poissonDiskDir);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SamplesBlock, poissonDiskHemisphere), sizeof(poissonDiskDirHemi2), poissonDiskDirHemi2);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, uboIDs[UBO_SAMPLES]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::setupPlaneMesh() {
	GLfloat quadVertices[] =
	{ -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	   0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
	   0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f
	};
	GLuint quadIndices[] = { 1, 0, 2, 0, 3, 2 };
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); //layout (location = 0) in vec3 aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1); //layout (location = 1) in vec2 aTexCoord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glGenBuffers(1, &quadEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
	glBindVertexArray(0);
}



void Renderer::setupCubeMesh() {
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glBindVertexArray(0);
}


void Renderer::setupSkybox() {
	GLubyte* imagePtr = nullptr;
	std::vector<std::string> paths{
		"C:/Users/juice/Documents/Graphics/Textures/skybox/skybox/right.jpg",
		"C:/Users/juice/Documents/Graphics/Textures/skybox/skybox/left.jpg",
		"C:/Users/juice/Documents/Graphics/Textures/skybox/skybox/top.jpg",
		"C:/Users/juice/Documents/Graphics/Textures/skybox/skybox/bottom.jpg",
		"C:/Users/juice/Documents/Graphics/Textures/skybox/skybox/front.jpg",
		"C:/Users/juice/Documents/Graphics/Textures/skybox/skybox/back.jpg"
	};
	skyboxShader.loadFile("Shaders/Skybox.vert", "Shaders/Skybox.frag");
	skyboxShader.useProgram();
	skyboxShader.setMat4("projection", glm::value_ptr(projection));
	skyboxShader.setInt("cubemap", 0);
	glGenTextures(1, &skyboxCubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapID);
	stbi_set_flip_vertically_on_load(false);
	assert(paths.size() == 6);
	for (int i = 0; i < 6; i++) {
		imagePtr = stbi_load(paths.at(i).c_str(), &skyboxImageWidth, &skyboxImageHeight,
			&skyboxImageChannels, 3);
		if (!imagePtr) {
			std::cout << "Error::Cubemap - Could not load face at path " << paths.at(i) << "\n";
			exit(EXIT_FAILURE);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, skyboxImageWidth,
			skyboxImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imagePtr);
		stbi_image_free(imagePtr);
		imagePtr = nullptr;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);
}


void Renderer::setupLightVolumes() {
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &lightVolumeModelBufID);
	glBindBuffer(GL_ARRAY_BUFFER, lightVolumeModelBufID);
	std::vector<glm::mat4> models;
	radii.resize(maxPointLights);
	for (int i = 0; i < lightPos.size(); i++) {
		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(lightPos.at(i)));
		glm::vec4 color = lightColor.at(i);
		float lightMax = std::max({ color.r, color.g, color.b });
		float constant = lightParam.at(i).x;
		float linear = lightParam.at(i).y;
		float quadratic = lightParam.at(i).z;
		float radius =
			(-linear + std::sqrtf(linear * linear - 4.0f * quadratic * (constant - (256.0f / 5.0f) * (lightMax / (lightMax + exposure)))))
			/ (2.0f * quadratic);
		radii.at(i) = radius;
		model = glm::scale(model, glm::vec3(radius));
		models.push_back(model);
	}
	glBufferData(GL_ARRAY_BUFFER, maxPointLights * sizeof(glm::mat4), glm::value_ptr(models[0])
		, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	lightVolumeShader.loadFile("Shaders/Deferred_lv_bp.vert", "Shaders/Deferred_lv_general.frag");
	lightVolumeShader.useProgram();
	lightVolumeShader.setInt("gPosition", 0);
	lightVolumeShader.setInt("gNormal", 1);
	lightVolumeShader.setInt("gAlbedo", 2);
	lightVolumeShader.setInt("shadowMaps", 3);
	lightVolumeShader.setInt("noise", 4);
	lightVolumeShader.setInt("gSpecularRoughness", 5);
	lightVolumeShader.setInt("SSAO", 6);
	lightVolumeShader.setBool("containsShadow", true);
	lightVolumeShader.setFloat("shadowFar", shadow_far);
	lightVolumeShader.setFloat("ambientStrength", ambientStrength);
	glm::vec2 windowSize(SCR_WIDTH, SCR_HEIGHT);
	lightVolumeShader.setVec2("windowSize", glm::value_ptr(windowSize));
	lightVolumeShader.setInt("shadingMode", 0);
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

void Renderer::setupCascadedShadowMaps() {
	glGenFramebuffers(1, &cascadedShadowFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, cascadedShadowFramebufferID);
	glGenTextures(1, &cascadedShadowTextureArrayID);
	GLuint shadowCascadeWidth = directionalShadowWidth, shadowCascadeHeight = directionalShadowHeight;
	glBindTexture(GL_TEXTURE_2D_ARRAY, cascadedShadowTextureArrayID);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32,
		shadowCascadeWidth, shadowCascadeHeight, numShadowCascades);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, shadowFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, shadowFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	shadowSplitDepths[0] = near;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cascadedShadowTextureArrayID, 0);
	cascadedShadowBounds[0] = glm::vec3(left, top, -near); //upper left
	cascadedShadowBounds[1] = glm::vec3(right, top, -near); //upper right
	cascadedShadowBounds[2] = glm::vec3(left, bottom, -near); //bottom left
	cascadedShadowBounds[3] = glm::vec3(right, bottom, -near); //bottom right
	cascadedShadowViewports.push_back(0.0f);
	cascadedShadowViewports.push_back(0.0f);
	cascadedShadowViewports.push_back(static_cast<GLfloat>(shadowCascadeWidth));
	cascadedShadowViewports.push_back(static_cast<GLfloat>(shadowCascadeHeight));
	GLfloat splitScale;
	for (int i = 1; i < numShadowCascades; i++) {
		//shadowCascadeWidth = std::max(shadowCascadeWidth / 2, 1u);
		//shadowCascadeHeight = std::max(shadowCascadeHeight / 2, 1u);
		cascadedShadowViewports.push_back(0.0f);
		cascadedShadowViewports.push_back(0.0f);
		cascadedShadowViewports.push_back(static_cast<GLfloat>(shadowCascadeWidth));
		cascadedShadowViewports.push_back(static_cast<GLfloat>(shadowCascadeHeight));
		shadowSplitDepths[i] = shadowSplitLinearFactor * near * std::powf(far / near, static_cast<GLfloat>(i) / static_cast<GLfloat>(numShadowCascades)) +
			(1.0f - shadowSplitLinearFactor) * depthSplitFuncLinear(near, far, numShadowCascades, i);
		splitScale = shadowSplitDepths[i] / near;
		cascadedShadowBounds[4 * i + 0] = splitScale * glm::vec3(left, top, -near); //upper left
		cascadedShadowBounds[4 * i + 1] = splitScale * glm::vec3(right, top, -near); //upper right
		cascadedShadowBounds[4 * i + 2] = splitScale * glm::vec3(left, bottom, -near); //bottom left
		cascadedShadowBounds[4 * i + 3] = splitScale * glm::vec3(right, bottom, -near); //bottom right
	}
	shadowSplitDepths[numShadowCascades] = far;
	splitScale = far / near;
	cascadedShadowBounds[4 * numShadowCascades + 0] = splitScale * glm::vec3(left, top, -near); //upper left
	cascadedShadowBounds[4 * numShadowCascades + 1] = splitScale * glm::vec3(right, top, -near); //upper right
	cascadedShadowBounds[4 * numShadowCascades + 2] = splitScale * glm::vec3(left, bottom, -near); //bottom left
	cascadedShadowBounds[4 * numShadowCascades + 3] = splitScale * glm::vec3(right, bottom, -near); //bottom right
	glDrawBuffer(GL_NONE);
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	cascadedShadowMapShader.loadFile("Shaders/ShadowMapCascade.vert", "Shaders/ShadowMapCascade.frag",
		"Shaders/ShadowMapCascade.geom");
	cascadedShadowMapShader.useProgram();
	cascadedShadowMapShader.setInt("numShadowCascades", numShadowCascades);
}



void Renderer::setupDirectionalLightShader() {
	directionalAmbientShader.loadFile("Shaders/Quad_framebuffer.vert", "Shaders/Deferred_general_ambient_dir.frag");
	directionalAmbientShader.useProgram();
	directionalAmbientShader.setInt("gPosition", 0);
	directionalAmbientShader.setInt("gNormal", 1);
	directionalAmbientShader.setInt("gAlbedo", 2);
	directionalAmbientShader.setInt("gDepth", 3);
	directionalAmbientShader.setInt("shadowCascades", 4);
	directionalAmbientShader.setInt("gSpecularRoughness", 5);
	directionalAmbientShader.setInt("SSAO", 6);
	directionalAmbientShader.setInt("numShadowCascades", numShadowCascades);
	glUniform1fv(glGetUniformLocation(directionalAmbientShader.program, "shadowSplitDepths"),
		numShadowCascades + 1, shadowSplitDepths);
	directionalAmbientShader.setBool("containsShadow", true);
	directionalAmbientShader.setFloat("shadowFar", shadow_far);
	directionalAmbientShader.setFloat("ambientStrength", ambientStrength);
	directionalAmbientShader.setInt("shadingMode", 0);
}

void Renderer::setupPointShadowMaps() {
	glGenTextures(1, &shadowCubemapArrayID);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubemapArrayID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, shadowFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, shadowFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1,
		GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight, 6 * maxShadowedPointLights);
	glGenFramebuffers(1, &shadowFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebufferID);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubemapArrayID, 0);
	shadowMapShader.loadFile("Shaders/ShadowMap.vert", "Shaders/ShadowMap.frag", "Shaders/ShadowMap.geom");
	/*
	glm::mat4 shadowProj(0.0f);
	shadowProj[0][0] = (1.0f) / (shadow_aspect * tanf(glm::radians(90.0f)/2.0f));
	shadowProj[1][1] = (1.0f) / tanf(glm::radians(90.0f) / 2.0f);
	shadowProj[2][2] = -(shadow_near + shadow_far) / (shadow_far - shadow_near);
	shadowProj[2][3] = -(1.0f);
	shadowProj[3][2] = -((2.0f) * shadow_far * shadow_near) / (shadow_far - shadow_near);
	*/
	/*
	glm::mat4 shadowProj(0.0f);
	shadowProj[0][0] = shadow_near;
	shadowProj[1][1] = shadow_near;
	shadowProj[2][2] = -(shadow_near + shadow_far);
	shadowProj[2][3] = -(1.0f);
	shadowProj[3][2] = -(shadow_far * shadow_near);
	*/
	//shadowProj = glm::ortho(-shadow_near, shadow_near,
	//-shadow_near, shadow_near) * shadowProj;
	shadowMapShader.useProgram();
	shadowMapShader.setFloat("shadowFar", shadow_far);
	shadowMapShader.setInt("numShadowedLights", numShadowedLights);
	for (int i = 0; i < numPointLights; i++) {
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), shadow_aspect, shadow_near, radii.at(i));
		shadowMatrices.push_back(shadowProj *
			glm::lookAt(glm::vec3(lightPos.at(i)), glm::vec3(lightPos.at(i)) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowMatrices.push_back(shadowProj *
			glm::lookAt(glm::vec3(lightPos.at(i)), glm::vec3(lightPos.at(i)) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowMatrices.push_back(shadowProj *
			glm::lookAt(glm::vec3(lightPos.at(i)), glm::vec3(lightPos.at(i)) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowMatrices.push_back(shadowProj *
			glm::lookAt(glm::vec3(lightPos.at(i)), glm::vec3(lightPos.at(i)) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowMatrices.push_back(shadowProj *
			glm::lookAt(glm::vec3(lightPos.at(i)), glm::vec3(lightPos.at(i)) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowMatrices.push_back(shadowProj *
			glm::lookAt(glm::vec3(lightPos.at(i)), glm::vec3(lightPos.at(i)) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	}
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_SHADOW_MATRICES]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 6 * numPointLights * sizeof(glm::mat4), glm::value_ptr(shadowMatrices[0]));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void generateRandom3DTexture(float* arr, size_t len) { //uniform over a sphere
	//auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	int seed = 984243;
	std::default_random_engine engine(seed);
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	for (int i = 0; i < len * len * len; i++) {
		arr[2*i] = 2 * pi * distribution(engine);
		arr[2 * i + 1] = glm::acos(1.0f - 2 * distribution(engine));
	}
}

void Renderer::setupRandomTexture()
{ //map world position to random angle so there is no flickering during camera movement
	generateRandom3DTexture(randomAngleTexture, randomTextureSize);
	glGenTextures(1, &randomTextureID);
	glBindTexture(GL_TEXTURE_3D, randomTextureID);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RG16F, randomTextureSize, randomTextureSize, randomTextureSize, 0,
		GL_RG, GL_FLOAT, randomAngleTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glBindTexture(GL_TEXTURE_3D, 0);
}





void Renderer::setupDeferredFramebuffer() {
	quadShader.loadFile("Shaders/Quad_framebuffer.vert", "Shaders/Quad_framebuffer.frag");
	quadShader.useProgram();
	quadShader.setFloat("far", far);
	quadShader.setInt("tex", 0);
	quadShader.setInt("numShadowCascades", numShadowCascades);
	glUniform1fv(glGetUniformLocation(quadShader.program, "shadowSplitDepths"), numShadowCascades + 1, shadowSplitDepths);
	glGenFramebuffers(1, &deferredFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFramebufferID);
	for (int i = 0; i < deferredAttachmentFormats.size(); i++) {
		GLuint colorTextureID = 0;
		glGenTextures(1, &colorTextureID);
		glBindTexture(GL_TEXTURE_2D, colorTextureID);
		GLint internalFormat = deferredAttachmentFormats.at(i);
		GLint type = GL_FLOAT;
		GLint format = GL_RGBA;
		if (internalFormat == GL_RGBA16F) type = GL_FLOAT;
		else if (internalFormat == GL_RGBA) type = GL_UNSIGNED_BYTE;
		else if (internalFormat == GL_R16F || internalFormat == GL_R32F || internalFormat == GL_R8) {
			type = GL_FLOAT;
			format = GL_RED;
		}
		else if (internalFormat == GL_RED) {
			type = GL_UNSIGNED_BYTE;
			format = GL_RED;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, deferredFramebufferWidth, deferredFramebufferHeight,
			0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
			colorTextureID, 0);
		deferredColorTextureIDs.push_back(colorTextureID);
		deferredAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glGenRenderbuffers(1, &deferredDepthRenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, deferredDepthRenderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		deferredFramebufferWidth, deferredFramebufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, deferredDepthRenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glDrawBuffers(deferredAttachments.size(), &deferredAttachments[0]);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: Deferred framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::setupOcclusionQueries()
{
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFramebufferID);
	occlusionShader.loadFile("Shaders/Basic.vert", "Shaders/Null.frag");
	ViewCompareModel compareFunc{ view };
	std::sort(modelAssets.begin(), modelAssets.end(), compareFunc);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	for (int i = 0; i < modelAssets.size(); i++) {
		modelAssets.at(i)->draw(occlusionShader, usePBR);
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	for (int i = 0; i < modelAssets.size(); i++) {
		modelAssets.at(i)->beginOcclusionQueries(occlusionShader, cubeVAO, view);
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::setupSSAO()
{
	glGenFramebuffers(1, &SSAOFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFramebufferID);
	glGenTextures(1, &SSAOTextureID);
	glBindTexture(GL_TEXTURE_2D, SSAOTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, SSAOTextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: SSAO framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glGenFramebuffers(1, &SSAOBlurFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFramebufferID);
	glGenTextures(1, &SSAOBlurTextureID);
	glBindTexture(GL_TEXTURE_2D, SSAOBlurTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, SSAOBlurTextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: SSAO blur framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	SSAOShader.loadFile("Shaders/Quad_framebuffer.vert", "Shaders/SSAO.frag");
	SSAOShader.useProgram();
	SSAOShader.setInt("gDepth", 0);
	SSAOShader.setInt("gPosition", 1);
	SSAOShader.setInt("gNormal", 2);
	SSAOShader.setInt("noise", 3);
	SSAOShader.setInt("numSamples", numSSAOSamples);
	SSAOShader.setFloat("radius", SSAOSampleRadius);
	SSAOBlurShader.loadFile("Shaders/Quad_framebuffer.vert", "Shaders/Bilateral.frag");
	SSAOBlurShader.useProgram();
	SSAOBlurShader.setInt("screen", 0);
}

void Renderer::setupDeinterleavedSSAO() {
	glGenFramebuffers(1, &deinterleavedSSAOFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, deinterleavedSSAOFramebufferID);
	glGenTextures(NUM_INTERLEAVED_NAMES, deinterleavedSSAOTextureIDs);
	std::vector<GLuint> drawBuffers;
	for (int i = 0; i < NUM_INTERLEAVED_NAMES; i++) {
		glBindTexture(GL_TEXTURE_2D, deinterleavedSSAOTextureIDs[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH / 2, SCR_HEIGHT / 2, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, deinterleavedSSAOTextureIDs[i], 0);
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: SSAO deinterleave framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glGenFramebuffers(1, &SSAOFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFramebufferID);
	glGenTextures(1, &SSAOTextureID);
	glBindTexture(GL_TEXTURE_2D, SSAOTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, SSAOTextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: SSAO reinterleave framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glGenFramebuffers(1, &SSAOBlurFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFramebufferID);
	glGenTextures(1, &SSAOBlurTextureID);
	glBindTexture(GL_TEXTURE_2D, SSAOBlurTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, SSAOBlurTextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: SSAO blur framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	deinterleavedSSAOShader.loadFile("Shaders/Quad_framebuffer.vert", "Shaders/SSAODeinterleaved.frag", "Shaders/SSAODeinterleaved.geom");
	deinterleavedSSAOShader.useProgram();
	deinterleavedSSAOShader.setInt("gDepth", 0);
	deinterleavedSSAOShader.setInt("gPosition", 1);
	deinterleavedSSAOShader.setInt("gNormal", 2);
	deinterleavedSSAOShader.setInt("noise", 3);
	deinterleavedSSAOShader.setInt("numSamples", numSSAOSamples);
	deinterleavedSSAOShader.setFloat("radius", SSAOSampleRadius);
}


void Renderer::setupLightDisplay() {
	glBindVertexArray(cubeVAO);
	lightDisplayShader.loadFile("Shaders/LightDisplay.vert", "Shaders/LightDisplay.frag");
	lightDisplayShader.useProgram();
	lightDisplayShader.setBool("instanced", true);
	glm::mat4 lightDisplayModel = glm::scale(glm::mat4(1.0f), glm::vec3(0.01, 0.01, 0.01));
	lightDisplayShader.setMat4("model", glm::value_ptr(lightDisplayModel));
	glGenBuffers(1, &lightDisplayPositionBufID);
	glBindBuffer(GL_ARRAY_BUFFER, lightDisplayPositionBufID);
	glBufferData(GL_ARRAY_BUFFER, maxPointLights * sizeof(glm::vec4),
		glm::value_ptr(lightPos[0]), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
	glVertexAttribDivisor(9, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Renderer::setupSunDisplay() {
	sunShader.loadFile("Shaders/Sun.vert", "Shaders/Sun.frag");
}

void Renderer::setupTestSceneLights() {
	numPointLights = 2;
	numShadowedLights = 2;
	/*
	lightPos.push_back(glm::vec4(3.0, 3.0, 3.0, 1.0));
	lightPos.push_back(glm::vec4(1.94, 1.65, -2.38, 1.0));
	lightColor.push_back(glm::vec4(3.0, 3.0, 3.0, 1.0));
	lightColor.push_back(glm::vec4(0.6, 0.6, 0.6, 1.0));
	lightParam.push_back(glm::vec4(lightConstant, lightLinear, lightQuadratic, specularExponent));
	lightParam.push_back(glm::vec4(lightConstant, lightLinear, lightQuadratic, specularExponent));
	*/
	/*
	lightPos.push_back(glm::vec4(0.1, 0.56, 2.17, 1.0));
	lightPos.push_back(glm::vec4(-0.6, 0.55, -4.67, 1.0));
	lightColor.push_back(glm::vec4(70, 70, 70, 1.0));
	lightColor.push_back(glm::vec4(50, 50, 50, 1.0));
	lightParam.push_back(glm::vec4(lightConstant, lightLinear, lightQuadratic, specularExponent));
	lightParam.push_back(glm::vec4(lightConstant, lightLinear, lightQuadratic, specularExponent));
	lightDir = glm::vec4(cos(lightDirTheta) * cos(lightDirPhi),
		sin(lightDirTheta),
		-sin(lightDirPhi) * cos(lightDirTheta),
		0.0f);
	lightDirColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	*/
	lightPos.push_back(glm::vec4(0.1, 0.56, 2.17, 1.0));
	lightPos.push_back(glm::vec4(-0.6, 0.55, -4.67, 1.0));
	lightColor.push_back(glm::vec4(70, 70, 70, 1.0));
	lightColor.push_back(glm::vec4(50, 50, 50, 1.0));
	lightParam.push_back(glm::vec4(lightConstant, lightLinear, lightQuadratic, specularExponent));
	lightParam.push_back(glm::vec4(lightConstant, lightLinear, lightQuadratic, specularExponent));
	lightDir = glm::vec4(cos(lightDirTheta) * cos(lightDirPhi),
		sin(lightDirTheta),
		-sin(lightDirPhi) * cos(lightDirTheta),
		0.0f);
	lightDirColor = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);
	setupLightVolumes();
	ambientStrength = 0.09f;
	exposure = 5.04f;
	usePBR = true;
	//theta = 1.5555
}


void Renderer::setupPostprocessFramebuffer()
{
	glGenFramebuffers(1, &postprocessFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, postprocessFramebufferID);
	glGenTextures(1, &postprocessColorTextureID);
	glBindTexture(GL_TEXTURE_2D, postprocessColorTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, postprocessColorTextureID, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenRenderbuffers(1, &postprocessRenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, postprocessRenderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, postprocessRenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER: Postprocess framebuffer is not complete!" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	postprocessShader.loadFile("Shaders/Quad_framebuffer.vert", "Shaders/Postprocess.frag");
	postprocessShader.useProgram();
	postprocessShader.setInt("screen", 0);
	glUseProgram(0);
}

void Renderer::setupShadowTesting() {
	shadowCascadeTest.setupBuffer(numShadowCascades);
	shadowCascadeTest.fillBuffer(numShadowCascades, cascadedShadowBounds);
	shadowTestShader.loadFile("Shaders/Quad_framebuffer.vert", "Shaders/Shadow.frag");
	shadowTestShader.useProgram();
	shadowTestShader.setInt("gPosition", 0);
	shadowTestShader.setInt("shadowMaps", 1);
	shadowTestShader.setBool("containsShadow", true);
	shadowTestShader.setFloat("shadowFar", shadow_far);
	shadowTestShader.setInt("numShadowCascades", numShadowCascades);
	shadowTestShader.setInt("gDepth", 2);
	shadowTestShader.setInt("shadowCascades", 3);
	glUniform1fv(glGetUniformLocation(shadowTestShader.program, "shadowSplitDepths"),
		numShadowCascades + 1, shadowSplitDepths);
	shadowCascadeTestShader.loadFile("Shaders/CascadeFrustum.vert", "Shaders/CascadeFrustum.frag");
	shadowCascadeTestShader.useProgram();
	shadowCascadeTestShader.setInt("gNormal", 4);
	shadowCascadeTestShader.setInt("noise", 5);
	glGenTextures(1, &omniShadowMapViewID);
	glTextureView(omniShadowMapViewID, GL_TEXTURE_CUBE_MAP, shadowCubemapArrayID, GL_DEPTH_COMPONENT24,
		0, 1, 0, 6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, omniShadowMapViewID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, shadowFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, shadowFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_NONE);
}

void Renderer::setupTestAxis() {
	testAxis.init();
	testAxis.setMat4("model", glm::value_ptr(glm::mat4(1.0f)));
}

void Renderer::setupWireframeShader() {
	wireframeShader.loadFile("Shaders/Wireframe.vert", "Shaders/Wireframe.frag");
}


void Renderer::setupTestScenePlane() { //old
	GLubyte* imagePtr = nullptr;
	quadModel = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	quadModel = glm::scale(quadModel, glm::vec3(planeScale, planeScale, 1.0f));
	planeShader.loadFile("Shaders/Quad.vert", "Shaders/Quad_deferred.frag");
	planeShader.useProgram();
	planeShader.setVec3("normal_in", quadNormal);
	planeShader.setMat4("model", glm::value_ptr(quadModel));
	planeShader.setFloat("texScale", planeScale);
	const char* texturePath = "C:/Users/juice/Documents/Graphics/Textures/wood.jpg";
	imagePtr = stbi_load(texturePath,
		&planeTextureWidth, &planeTextureHeight, &planeTextureChannels, 0);
	if (!imagePtr) {
		std::cout << "Unable to load image at path: " << texturePath << std::endl;
		exit(EXIT_FAILURE);
	}
	glGenTextures(1, &planeTextureID);
	glBindTexture(GL_TEXTURE_2D, planeTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, (planeTextureChannels == 4) ? GL_SRGB8_ALPHA8 : GL_SRGB8, planeTextureWidth,
		planeTextureHeight, 0, (planeTextureChannels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imagePtr);
	stbi_image_free(imagePtr); imagePtr = nullptr;
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	planeShader.setInt("tex", 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Renderer::setupModelShader()
{
	modelShader.loadFile("Shaders/General_deferred.vert", "Shaders/General_deferred.frag");
	modelShader.useProgram();
}

void Renderer::setupTestSceneModel() {
	/*
	Model* testModel = new Model;
	testModel->loadModel("C:\\Users\\juice\\Documents\\Graphics\\Models\\sponza\\sponza.obj", false, false, false);
	testModel->scale = glm::vec3(0.01f, 0.01f, 0.01f);
	modelAssets.push_back(testModel);
	*/
	
	Model* testModel = new Model;
	testModel->loadModel("C:\\Users\\juice\\Documents\\Graphics\\Models\\Gledista_Triacanthos\\Gledista_Triacanthos_OBJ\\Gledista_Triacanthos.obj", false, false, false);
	testModel->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	testModel->position = glm::vec4(0.0f, -3.0f, -8.0f, 1.0f);
	modelAssets.push_back(testModel);
	Model* testModel2 = new Model;
	testModel2->loadModel("C:/Users/juice/Documents/Graphics/Models/grassPlane/grassPlane.obj", true, false, false);
	testModel2->position = glm::vec4(0.0f, -3.0f, 0.0f, 1.0f);
	modelAssets.push_back(testModel2);
	Model* testModel3 = new Model;
	testModel3->loadModel("C:\\Users\\juice\\Documents\\Graphics\\Models\\WoodenCabinObj\\WoodenCabinOBJ.obj", false, false, false);
	testModel3->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	testModel3->position = glm::vec4(0.0f, -3.0f, 0.0f, 1.0f);
	modelAssets.push_back(testModel3);
	
}

void Renderer::renderTestSceneDeferredPass() { //old
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFramebufferID);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, &far);
	glActiveTexture(GL_TEXTURE0);
	if (!modelAssets.empty()) if (modelAssets.at(0)) modelAssets.at(0)->draw(modelShader, usePBR);
	planeShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTextureID);
	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderDeferredPass()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFramebufferID);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, &far);
	glActiveTexture(GL_TEXTURE0);
	if (useOcclusionCulling) {
		ViewCompareModel compareFunc{ view };
		std::sort(modelAssets.begin(), modelAssets.end(), compareFunc);
	}
	modelShader.useProgram();
	for (int i = 0; i < modelAssets.size(); i++) {
		Model* curModel = modelAssets.at(i);
		if (useOcclusionCulling) curModel->drawOcclusionCulling(modelShader, cubeVAO, view, occlusionShader, usePBR);
		else curModel->draw(modelShader, usePBR);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::renderTestSceneShadowMapCascades() { //old
	glm::mat4 dirLightView = glm::lookAt(camera.pos, camera.pos - lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
	cameraViewToWorld = glm::inverse(view);
	lightViewtoWorld = glm::inverse(dirLightView);
	//using a bounding box to construct view and orthographic matrices
	//gets rid of shimmering due to camera rotation
	for (int i = 0; i < numShadowCascades; i++) {
		worldShadowBounds[4 * i + 0] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 0], 1.0f);
		worldShadowBounds[4 * i + 1] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 1], 1.0f);
		worldShadowBounds[4 * i + 2] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 2], 1.0f);
		worldShadowBounds[4 * i + 3] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 3], 1.0f);
		worldShadowBounds[4 * i + 4] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 4], 1.0f);
		worldShadowBounds[4 * i + 5] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 5], 1.0f);
		worldShadowBounds[4 * i + 6] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 6], 1.0f);
		worldShadowBounds[4 * i + 7] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 7], 1.0f);
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (int j = 4 * i; j < ((4 * i) + 8); j++) {
			frustumCenter += worldShadowBounds[j];
		}
		frustumCenter /= 8.0f;
		//placing view at center of bounding box helps reduce quantization errors
		glm::mat4 dirLightView = glm::lookAt(frustumCenter, frustumCenter - lightDir, glm::vec3(0.0f, 1.0f, 0.0f));


		GLfloat maxDistance =
			std::max(
				{ glm::length(frustumCenter - worldShadowBounds[4 * i + 0]), glm::length(frustumCenter - worldShadowBounds[4 * i + 4]),
				 glm::length(frustumCenter - worldShadowBounds[4 * i + 1]), glm::length(frustumCenter - worldShadowBounds[4 * i + 1 + 4]),
				 glm::length(frustumCenter - worldShadowBounds[4 * i + 2]), glm::length(frustumCenter - worldShadowBounds[4 * i + 2 + 4]),
				 glm::length(frustumCenter - worldShadowBounds[4 * i + 3]), glm::length(frustumCenter - worldShadowBounds[4 * i + 3 + 4]) }
		);


		//displaying purposes for debugging
		cascadedShadowBoxBounds[8 * i + 0] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, 1, 1)));
		cascadedShadowBoxBounds[8 * i + 1] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, 1, 1)));
		cascadedShadowBoxBounds[8 * i + 2] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, -1, 1)));
		cascadedShadowBoxBounds[8 * i + 3] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, -1, 1)));
		cascadedShadowBoxBounds[8 * i + 4] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, 1, -1)));
		cascadedShadowBoxBounds[8 * i + 5] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, 1, -1)));
		cascadedShadowBoxBounds[8 * i + 6] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, -1, -1)));
		cascadedShadowBoxBounds[8 * i + 7] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, -1, -1)));

		frustumCenter = dirLightView * glm::vec4(frustumCenter, 1.0f);

		glm::mat4 cascadedOrtho = glm::ortho(frustumCenter.x - maxDistance, frustumCenter.x + maxDistance,
			frustumCenter.y - maxDistance, frustumCenter.y + maxDistance,
			frustumCenter.z - maxDistance, frustumCenter.z + maxDistance);
		//reduce shimmering from moving camera
		glm::vec4 arbitraryPoint(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 arbitraryPointTexCoord = cascadedOrtho * dirLightView * arbitraryPoint;
		arbitraryPointTexCoord *= (directionalShadowWidth / 2.0f);
		glm::vec4 shadowTexCoordOffset = glm::round(arbitraryPointTexCoord) - arbitraryPointTexCoord;
		shadowTexCoordOffset *= (2.0f / directionalShadowWidth);
		cascadedOrtho[3] += glm::vec4(shadowTexCoordOffset.x, shadowTexCoordOffset.y, 0.0f, 0.0f);

		cascadedShadowMatrices[i] = cascadedOrtho * dirLightView;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, cascadedShadowFramebufferID);
	cascadedShadowMapShader.useProgram();
	glViewportArrayv(0, numShadowCascades, cascadedShadowViewports.data());
	glUniformMatrix4fv(glGetUniformLocation(cascadedShadowMapShader.program, "shadowMatrices"),
		numShadowCascades, GL_FALSE, glm::value_ptr(cascadedShadowMatrices[0]));
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	if (!modelAssets.empty()) if (modelAssets.at(0)) modelAssets.at(0)->draw(cascadedShadowMapShader, usePBR);
	glBindVertexArray(quadVAO);
	cascadedShadowMapShader.setMat4("model", glm::value_ptr(quadModel));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderShadowMapCascades()
{
	glm::mat4 dirLightView = glm::lookAt(camera.pos, camera.pos - lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
	cameraViewToWorld = glm::inverse(view);
	lightViewtoWorld = glm::inverse(dirLightView);
	//using a bounding box to construct view and orthographic matrices
	//gets rid of shimmering due to camera rotation
	for (int i = 0; i < numShadowCascades; i++) {
		worldShadowBounds[4 * i + 0] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 0], 1.0f);
		worldShadowBounds[4 * i + 1] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 1], 1.0f);
		worldShadowBounds[4 * i + 2] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 2], 1.0f);
		worldShadowBounds[4 * i + 3] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 3], 1.0f);
		worldShadowBounds[4 * i + 4] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 4], 1.0f);
		worldShadowBounds[4 * i + 5] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 5], 1.0f);
		worldShadowBounds[4 * i + 6] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 6], 1.0f);
		worldShadowBounds[4 * i + 7] = cameraViewToWorld * glm::vec4(cascadedShadowBounds[4 * i + 7], 1.0f);
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (int j = 4 * i; j < ((4 * i) + 8); j++) {
			frustumCenter += worldShadowBounds[j];
		}
		frustumCenter /= 8.0f;
		//placing view at center of bounding box helps reduce quantization errors
		glm::mat4 dirLightView = glm::lookAt(frustumCenter, frustumCenter - lightDir, glm::vec3(0.0f, 1.0f, 0.0f));


		GLfloat maxDistance =
			std::max(
				{ glm::length(frustumCenter - worldShadowBounds[4 * i + 0]), glm::length(frustumCenter - worldShadowBounds[4 * i + 4]),
				 glm::length(frustumCenter - worldShadowBounds[4 * i + 1]), glm::length(frustumCenter - worldShadowBounds[4 * i + 1 + 4]),
				 glm::length(frustumCenter - worldShadowBounds[4 * i + 2]), glm::length(frustumCenter - worldShadowBounds[4 * i + 2 + 4]),
				 glm::length(frustumCenter - worldShadowBounds[4 * i + 3]), glm::length(frustumCenter - worldShadowBounds[4 * i + 3 + 4]) }
		);


		//displaying purposes for debugging
		cascadedShadowBoxBounds[8 * i + 0] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, 1, 1)));
		cascadedShadowBoxBounds[8 * i + 1] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, 1, 1)));
		cascadedShadowBoxBounds[8 * i + 2] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, -1, 1)));
		cascadedShadowBoxBounds[8 * i + 3] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, -1, 1)));
		cascadedShadowBoxBounds[8 * i + 4] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, 1, -1)));
		cascadedShadowBoxBounds[8 * i + 5] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, 1, -1)));
		cascadedShadowBoxBounds[8 * i + 6] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(-1, -1, -1)));
		cascadedShadowBoxBounds[8 * i + 7] = frustumCenter + ((maxDistance)*glm::normalize(glm::vec3(1, -1, -1)));

		frustumCenter = dirLightView * glm::vec4(frustumCenter, 1.0f);

		glm::mat4 cascadedOrtho = glm::ortho(frustumCenter.x - maxDistance, frustumCenter.x + maxDistance,
			frustumCenter.y - maxDistance, frustumCenter.y + maxDistance,
			frustumCenter.z - maxDistance, frustumCenter.z + maxDistance);
		//reduce shimmering from moving camera
		glm::vec4 arbitraryPoint(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 arbitraryPointTexCoord = cascadedOrtho * dirLightView * arbitraryPoint;
		arbitraryPointTexCoord *= (directionalShadowWidth / 2.0f);
		glm::vec4 shadowTexCoordOffset = glm::round(arbitraryPointTexCoord) - arbitraryPointTexCoord;
		shadowTexCoordOffset *= (2.0f / directionalShadowWidth);
		cascadedOrtho[3] += glm::vec4(shadowTexCoordOffset.x, shadowTexCoordOffset.y, 0.0f, 0.0f);

		cascadedShadowMatrices[i] = cascadedOrtho * dirLightView;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, cascadedShadowFramebufferID);
	cascadedShadowMapShader.useProgram();
	glViewportArrayv(0, numShadowCascades, cascadedShadowViewports.data());
	glUniformMatrix4fv(glGetUniformLocation(cascadedShadowMapShader.program, "shadowMatrices"),
		numShadowCascades, GL_FALSE, glm::value_ptr(cascadedShadowMatrices[0]));
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	for (int i = 0; i < modelAssets.size(); i++) {
		Model* curModel = modelAssets.at(i);
		curModel->draw(cascadedShadowMapShader, usePBR);
	}
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderTestScenePointShadowMaps() { //old
	shadowMapShader.useProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebufferID);
	glViewport(0, 0, shadowWidth, shadowHeight);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	if (!modelAssets.empty()) if (modelAssets.at(0)) modelAssets.at(0)->draw(shadowMapShader, usePBR);
	glBindVertexArray(quadVAO);
	shadowMapShader.setMat4("model", glm::value_ptr(quadModel));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderPointShadowMaps()
{
	shadowMapShader.useProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebufferID);
	glViewport(0, 0, shadowWidth, shadowHeight);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	for (int i = 0; i < modelAssets.size(); i++) {
		Model* curModel = modelAssets.at(i);
		curModel->draw(shadowMapShader, usePBR);
	}
	//glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderSSAO() {
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFramebufferID);
	SSAOShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(0)); //gDepth
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(1)); //gPosition
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(2)); //gNormal
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D, randomTextureID);
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFramebufferID);
	SSAOBlurShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SSAOTextureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderLightingPass() {
	glBindFramebuffer(GL_FRAMEBUFFER, postprocessFramebufferID);
	//separate lighting passes for deferred
	glDisable(GL_DEPTH_TEST);
	//ambient + directional pass
	directionalAmbientShader.useProgram();
	directionalAmbientShader.setFloat("ambientStrength", ambientStrength);
	if (usePBR) {
		directionalAmbientShader.setInt("shadingMode", 1);
	}
	else {
		directionalAmbientShader.setInt("shadingMode", 0);
	}
	glUniformMatrix4fv(glGetUniformLocation(directionalAmbientShader.program, "cascadedShadowMatrices"),
		numShadowCascades, GL_FALSE, glm::value_ptr(cascadedShadowMatrices[0]));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(1)); //gPosition
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(2)); //gNormal
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(3)); //gAlbedo
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(0)); //gDepth
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D_ARRAY, cascadedShadowTextureArrayID);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(4)); //gSpecularExponent
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, SSAOBlurTextureID);
	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	//point light volume pass
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	lightVolumeShader.useProgram();
	if (usePBR) {
		lightVolumeShader.setInt("shadingMode", 1);
	}
	else {
		lightVolumeShader.setInt("shadingMode", 0);
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //cube has clockwise winding
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubemapArrayID);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_3D, randomTextureID);
	glBindVertexArray(cubeVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numPointLights);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredFramebufferID);
	//draw objects not part of deferred pipeline
	glBlitFramebuffer(
		0, 0, deferredFramebufferWidth, deferredFramebufferHeight, 0, 0, SCR_WIDTH, SCR_HEIGHT,
		GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, deferredFramebufferWidth, deferredFramebufferHeight, 0, 0, SCR_WIDTH, SCR_HEIGHT,
		GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
}

void Renderer::drawFrustums() {
	shadowCascadeTestShader.useProgram();
	if (frustum_outline_mode == Renderer::VIEW_FRUSTUM_OUTLINE)
		shadowCascadeTestShader.setMat4("inverseView", glm::value_ptr(cameraViewToWorld));
	if (frustum_outline_mode == Renderer::CASCADE_FRUSTUM_OUTLINE) {
		glm::mat4 identity(1.0f);
		shadowCascadeTestShader.setMat4("inverseView", glm::value_ptr(identity));
	}
	shadowCascadeTest.draw();
}

void Renderer::drawSkybox() {
	glm::mat4 cubemapView;
	if (cameraMode == 0) cubemapView = glm::mat4(glm::mat3(view));
	if (cameraMode == 1) cubemapView = glm::mat4(glm::mat3(altView));
	skyboxShader.useProgram();
	skyboxShader.setMat4("view", glm::value_ptr(cubemapView)); //must set view for skybox since it has a separate view
	if (skybox_mode == SKYBOX_DEFAULT) { //draw default skybox
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapID);
		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		sunShader.useProgram();
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else if (skybox_mode == SKYBOX_SHADOW_MAP) { //draw one of the shadow cubemaps as skybox
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, omniShadowMapViewID);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		skyboxShader.useProgram();
		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		sunShader.useProgram();
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glBindVertexArray(0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

void Renderer::drawMiscObjects() {
	glBindFramebuffer(GL_FRAMEBUFFER, postprocessFramebufferID);
	lightDisplayShader.useProgram();
	glBindVertexArray(cubeVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numPointLights);
	glBindVertexArray(0);
}

void Renderer::fillShadowCascadeBuffer() {
	if (frustum_outline_mode == Renderer::VIEW_FRUSTUM_OUTLINE)
		shadowCascadeTest.fillBuffer(numShadowCascades, cascadedShadowBounds);
	if (frustum_outline_mode == Renderer::CASCADE_FRUSTUM_OUTLINE)
		shadowCascadeTest.fillBuffer(numShadowCascades, cascadedShadowBoxBounds);
}




void Renderer::updateViewUniformBuffer(glm::mat4& viewMatrix) {
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_MATRICES]);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(MatricesUniformBlock, view),
		sizeof(glm::mat4), glm::value_ptr(viewMatrix));
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_LIGHTS]);
	glm::vec4 paddedCameraPos = glm::vec4(camera.pos, 1.0f);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, cameraPos),
		sizeof(glm::vec4), glm::value_ptr(paddedCameraPos));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::updatePointLight(int index, glm::vec4* position, glm::vec4* color, glm::vec4* param)
{
	if (position) lightPos.at(index) = *position;
	if (color) lightColor.at(index) = *color;
	if (param) lightParam.at(index) = *param;
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_LIGHTS]);
	if (position) glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightPos) + index * sizeof(glm::vec4),
		sizeof(glm::vec4), glm::value_ptr(*position));
	if (color) 	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightColor) + index * sizeof(glm::vec4),
		sizeof(glm::vec4), glm::value_ptr(*color));
	if (param) 	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightParam) + index * sizeof(glm::vec4),
		sizeof(glm::vec4), glm::value_ptr(*param));
	glm::mat4 lightVolumeModel(1.0f);
	lightVolumeModel = glm::translate(lightVolumeModel, glm::vec3(lightPos.at(index)));
	glm::vec4 lightModelColor = lightColor.at(index);
	float lightMax = std::max({ lightModelColor.r, lightModelColor.g, lightModelColor.b });
	float constant = lightParam.at(index).x;
	float linear = lightParam.at(index).y;
	float quadratic = lightParam.at(index).z;
	float radius =
		(-linear + std::sqrtf(linear * linear - 4.0f * quadratic * (constant - (256.0f / 5.0f) * (lightMax / (lightMax+exposure)) )))
		/ (2.0f * quadratic);
	radii.at(index) = radius;
	lightVolumeModel = glm::scale(lightVolumeModel, glm::vec3(radius));
	glBindBuffer(GL_ARRAY_BUFFER, lightVolumeModelBufID);
	glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightVolumeModel));
	if (position) {
		if (index < numShadowedLights) {
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), shadow_aspect, shadow_near, radii.at(index));
			shadowMatrices.at(6 * index) = (shadowProj *
				glm::lookAt(glm::vec3(lightPos.at(index)), glm::vec3(lightPos.at(index)) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowMatrices.at(6 * index + 1) = (shadowProj *
				glm::lookAt(glm::vec3(lightPos.at(index)), glm::vec3(lightPos.at(index)) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowMatrices.at(6 * index + 2) = (shadowProj *
				glm::lookAt(glm::vec3(lightPos.at(index)), glm::vec3(lightPos.at(index)) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			shadowMatrices.at(6 * index + 3) = (shadowProj *
				glm::lookAt(glm::vec3(lightPos.at(index)), glm::vec3(lightPos.at(index)) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
			shadowMatrices.at(6 * index + 4) = (shadowProj *
				glm::lookAt(glm::vec3(lightPos.at(index)), glm::vec3(lightPos.at(index)) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowMatrices.at(6 * index + 5) = (shadowProj *
				glm::lookAt(glm::vec3(lightPos.at(index)), glm::vec3(lightPos.at(index)) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
			glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_SHADOW_MATRICES]);
			glBufferSubData(GL_UNIFORM_BUFFER, 6 * index * sizeof(glm::mat4), 6 * sizeof(glm::mat4), glm::value_ptr(shadowMatrices[6 * index]));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, lightDisplayPositionBufID);
		glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(*position));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	renderPointShadowMaps();
}

void Renderer::updateDirectionalLight(glm::vec4* newLightDir, glm::vec4* newLightDirColor)
{
	if (newLightDir) lightDir = *newLightDir;
	if (newLightDirColor) lightDirColor = *newLightDirColor;
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_LIGHTS]);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightDir), sizeof(glm::vec4), glm::value_ptr(lightDir));
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, lightDirColor), sizeof(glm::vec4), glm::value_ptr(lightDirColor));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::addModel(std::string path, bool importAsSingleMesh, bool flipUVs, bool hasPBR)
{
	Model* model = new Model;
	if (model->loadModel(path, importAsSingleMesh, flipUVs, hasPBR))
		modelAssets.push_back(model);
	else delete model;
	renderPointShadowMaps();
	setupOcclusionQueries();
}

void Renderer::removeModel(int index)
{
	assert(index >= 0 && index < modelAssets.size());
	delete modelAssets.at(index);
	modelAssets.erase(modelAssets.begin() + index);
	renderPointShadowMaps();
	setupOcclusionQueries();
}

void Renderer::pushPointLight()
{
	if (numPointLights < maxPointLights) {
		numPointLights++;
		lightPos.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		lightColor.push_back(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		lightParam.push_back(glm::vec4(lightConstant, lightLinear, lightQuadratic, specularExponent));
		glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_LIGHTS]);
		glm::ivec4 paddedNumLights(numPointLights, 0, 0, 0);
		glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, numLights),
			sizeof(glm::ivec4), glm::value_ptr(paddedNumLights));
		if (numPointLights <= numShadowedLights) {
			shadowMatrices.push_back(glm::mat4(1.0f));
			shadowMatrices.push_back(glm::mat4(1.0f));
			shadowMatrices.push_back(glm::mat4(1.0f));
			shadowMatrices.push_back(glm::mat4(1.0f));
			shadowMatrices.push_back(glm::mat4(1.0f));
			shadowMatrices.push_back(glm::mat4(1.0f));
		}
		updatePointLight(numPointLights - 1, &lightPos.at(numPointLights - 1),
			&lightColor.at(numPointLights - 1), &lightParam.at(numPointLights - 1));
	}
}

void Renderer::popPointLight()
{
	numPointLights--;
	glBindBuffer(GL_UNIFORM_BUFFER, uboIDs[UBO_LIGHTS]);
	glm::ivec4 paddedNumLights(numPointLights, 0, 0, 0);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetof(LightsUniformBlock, numLights),
		sizeof(glm::ivec4), glm::value_ptr(paddedNumLights));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	lightPos.pop_back();
	lightColor.pop_back();
	lightParam.pop_back();
	if (numPointLights < numShadowedLights) {
		numShadowedLights--;
		shadowMapShader.useProgram();
		shadowMapShader.setInt("numShadowedLights", std::min(numShadowedLights, maxShadowedPointLights));
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		renderPointShadowMaps();
	}
}

void Renderer::updateNumShadowedPointLights(int num) {
	int oldNumShadowedLights = numShadowedLights;
	numShadowedLights = std::min(num, numPointLights);
	if (numShadowedLights > oldNumShadowedLights) {
		shadowMatrices.push_back(glm::mat4(1.0f));
		shadowMatrices.push_back(glm::mat4(1.0f));
		shadowMatrices.push_back(glm::mat4(1.0f));
		shadowMatrices.push_back(glm::mat4(1.0f));
		shadowMatrices.push_back(glm::mat4(1.0f));
		shadowMatrices.push_back(glm::mat4(1.0f));
		updatePointLight(numPointLights - 1, &lightPos.at(numPointLights - 1),
			&lightColor.at(numPointLights - 1), &lightParam.at(numPointLights - 1));
		shadowMapShader.useProgram();
		shadowMapShader.setInt("numShadowedLights", std::min(numShadowedLights, maxShadowedPointLights));
		renderPointShadowMaps();
	}
	else if (numShadowedLights < oldNumShadowedLights) {
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMatrices.pop_back();
		shadowMapShader.useProgram();
		shadowMapShader.setInt("numShadowedLights", std::min(numShadowedLights, maxShadowedPointLights));
		renderPointShadowMaps();
	}
}

void Renderer::updateSSAOParameters(int numSamples, float radius) {
	numSSAOSamples = numSamples;
	SSAOSampleRadius = radius;
	SSAOShader.useProgram();
	SSAOShader.setInt("numSamples", numSSAOSamples);
	SSAOShader.setFloat("radius", SSAOSampleRadius);
}

void Renderer::setupTestScene() { //old
	setupPlaneMesh();
	setupCubeMesh();
	setupTestSceneLights();
	setupModelShader();
	setupTestSceneModel();
	setupTestScenePlane();
	setupUniformBuffers();
	setupRandomTexture();
	setupSkybox();
	setupCascadedShadowMaps();
	setupDirectionalLightShader();
	setupPointShadowMaps();
	setupDeferredFramebuffer();
	setupPostprocessFramebuffer();
	setupTestAxis();
	setupLightDisplay();
	setupShadowTesting();
	setupSunDisplay();
	setupWireframeShader();
}

void Renderer::setup()
{
	setupPlaneMesh();
	setupCubeMesh();
	setupTestSceneLights();
	setupModelShader();
	setupTestSceneModel();
	setupUniformBuffers();
	setupRandomTexture();
	setupSkybox();
	setupCascadedShadowMaps();
	setupDirectionalLightShader();
	setupPointShadowMaps();
	setupDeferredFramebuffer();
	setupPostprocessFramebuffer();
	setupSSAO();
	setupTestAxis();
	setupLightDisplay();
	setupShadowTesting();
	setupSunDisplay();
	setupWireframeShader();
	renderPointShadowMaps();
	setupOcclusionQueries();
}

void Renderer::renderTestScene() { //old
	if (render_mode == RENDER_WIREFRAME) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframeShader.useProgram();
		glBindVertexArray(quadVAO);
		wireframeShader.setMat4("model", glm::value_ptr(quadModel));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		testAxis.draw();
		wireframeShader.useProgram();
		wireframeShader.setBool("instanced", true);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindVertexArray(cubeVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numPointLights);
		glBindVertexArray(0);
		glDisable(GL_CULL_FACE);
		wireframeShader.setBool("instanced", false);
		if (!modelAssets.empty()) if (modelAssets.at(0)) modelAssets.at(0)->draw(wireframeShader, usePBR);
		glDepthFunc(GL_LEQUAL);
		sunShader.useProgram();
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		renderTestSceneDeferredPass();
		renderTestSceneShadowMapCascades();
		fillShadowCascadeBuffer();
		renderTestScenePointShadowMaps();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		if (render_mode == RENDER_DEFAULT) {
			glBindFramebuffer(GL_FRAMEBUFFER, postprocessFramebufferID);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			renderLightingPass();
			drawMiscObjects();
			if (frustum_outline_mode != NO_FRUSTUM_OUTLINE) drawFrustums();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_FRAMEBUFFER_SRGB);
			glDisable(GL_DEPTH_TEST);
			postprocessShader.useProgram();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, postprocessColorTextureID);
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_FRAMEBUFFER_SRGB);
		}
		else if (render_mode == RENDER_SHADOW) {
			glDisable(GL_DEPTH_TEST);
			shadowTestShader.useProgram();
			glUniformMatrix4fv(glGetUniformLocation(shadowTestShader.program, "cascadedShadowMatrices"),
				numShadowCascades, GL_FALSE, glm::value_ptr(cascadedShadowMatrices[0]));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(1)); //gPosition
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubemapArrayID);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(0));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D_ARRAY, cascadedShadowTextureArrayID);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(2));
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_3D, randomTextureID);
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredFramebufferID);
			glBlitFramebuffer(
				0, 0, deferredFramebufferWidth, deferredFramebufferHeight, 0, 0, SCR_WIDTH, SCR_HEIGHT,
				GL_DEPTH_BUFFER_BIT, GL_NEAREST
			);
			testAxis.draw();
			glActiveTexture(GL_TEXTURE0);
		}
		else if (render_mode == RENDER_POSITION) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(1));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_NORMAL) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(2));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_ALBEDO) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(3));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_DEPTH || render_mode == RENDER_CASCADE_DEPTHS) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(0));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_SPECULARITY) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(4));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
	}
}

void Renderer::render() {
	if (render_mode == RENDER_WIREFRAME) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glm::mat4 identity(1.0f);
		wireframeShader.useProgram();
		wireframeShader.setMat4("model", glm::value_ptr(identity));
		testAxis.draw();
		wireframeShader.useProgram();
		wireframeShader.setBool("instanced", true);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindVertexArray(cubeVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numPointLights);
		glBindVertexArray(0);
		glDisable(GL_CULL_FACE);
		wireframeShader.setBool("instanced", false);
		for (int i = 0; i < modelAssets.size(); i++) {
			Model* curModel = modelAssets.at(i);
			if (drawBoundingBoxes) curModel->drawBoundingBoxes(wireframeShader, cubeVAO);
			else curModel->draw(wireframeShader, usePBR);
		}
		glDepthFunc(GL_LEQUAL);
		sunShader.useProgram();
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		renderDeferredPass();
		if (render_mode == RENDER_DEFAULT) {
			renderSSAO();
			renderShadowMapCascades();
			fillShadowCascadeBuffer();
			//renderPointShadowMaps();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, postprocessFramebufferID);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			renderLightingPass();
			drawMiscObjects();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_FRAMEBUFFER_SRGB);
			glDisable(GL_DEPTH_TEST);
			postprocessShader.useProgram();
			postprocessShader.setFloat("exposure", exposure);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, postprocessColorTextureID);
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, 0);
			drawSkybox();
			glDisable(GL_FRAMEBUFFER_SRGB);
			if (frustum_outline_mode != NO_FRUSTUM_OUTLINE) drawFrustums();
			testAxis.draw();
		}
		else if (render_mode == RENDER_SHADOW) {
			renderShadowMapCascades();
			fillShadowCascadeBuffer();
			//renderPointShadowMaps();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glDisable(GL_DEPTH_TEST);
			shadowTestShader.useProgram();
			glUniformMatrix4fv(glGetUniformLocation(shadowTestShader.program, "cascadedShadowMatrices"),
				numShadowCascades, GL_FALSE, glm::value_ptr(cascadedShadowMatrices[0]));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(1)); //gPosition
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowCubemapArrayID);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(0));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D_ARRAY, cascadedShadowTextureArrayID);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(2));
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_3D, randomTextureID);
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredFramebufferID);
			glBlitFramebuffer(
				0, 0, deferredFramebufferWidth, deferredFramebufferHeight, 0, 0, SCR_WIDTH, SCR_HEIGHT,
				GL_DEPTH_BUFFER_BIT, GL_NEAREST
			);
			testAxis.draw();
			glActiveTexture(GL_TEXTURE0);
		}
		else if (render_mode == RENDER_POSITION) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(1));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_NORMAL) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(2));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_ALBEDO) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(3));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_DEPTH || render_mode == RENDER_CASCADE_DEPTHS) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(0));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_SPECULARITY || render_mode == RENDER_ROUGHNESS) {
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, deferredColorTextureIDs.at(4));
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
		else if (render_mode == RENDER_SSAO) {
			renderSSAO();
			glDisable(GL_DEPTH_TEST);
			quadShader.useProgram();
			quadShader.setInt("renderMode", render_mode);
			glActiveTexture(GL_TEXTURE0);
			if (showBlur) glBindTexture(GL_TEXTURE_2D, SSAOBlurTextureID);
			else glBindTexture(GL_TEXTURE_2D, SSAOTextureID);
			glBindVertexArray(quadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}
	}
}




//initialization of global variables related to rendering

glm::mat4 Renderer::projection;
glm::mat4 Renderer::infiniteProj;
glm::mat4 Renderer::view = glm::lookAt(camera.pos, camera.pos + camera.front, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 Renderer::altView = glm::lookAt(cameraAlt.pos, cameraAlt.pos + cameraAlt.front, glm::vec3(0.0f, 1.0f, 0.0f));

int Renderer::render_mode = Renderer::RENDER_DEFAULT;
int Renderer::skybox_mode = Renderer::SKYBOX_DEFAULT;
int Renderer::frustum_outline_mode = Renderer::NO_FRUSTUM_OUTLINE;
int Renderer::cameraMode = 0;

int Renderer::numPointLights = 0;
int Renderer::numShadowedLights = 0;
glm::vec4 Renderer::lightDirColor;
glm::vec3 Renderer::lightDir;
std::vector<glm::vec4> Renderer::lightPos;
std::vector<glm::vec4> Renderer::lightColor;
std::vector<glm::vec4> Renderer::lightParam;
int Renderer::numSSAOSamples = 23;
float Renderer::SSAOSampleRadius = 0.38f;
float Renderer::exposure = 1.0f;
float Renderer::ambientStrength = 0.01f;
bool Renderer::showBlur = false;
bool Renderer::drawBoundingBoxes = false;
bool Renderer::useOcclusionCulling = true;
bool Renderer::usePBR = false;
bool Renderer::useDeinterleavedSSAO = false;

//clockwise winding order
const GLfloat cube_vertices[108]{
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

const GLfloat quadNormal[3] = { 0.0f, 0.0f, 1.0f };

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
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
    indices.push_back(4 * numShadowCascades);
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

Renderer* Renderer::instance = nullptr;

std::vector<Model*> Renderer::modelAssets;

Renderer* Renderer::getInstance()
{
	if (instance == nullptr) {
		instance = new Renderer;
	}
	return instance;
}
