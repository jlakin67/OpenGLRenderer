#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>

class SceneLoader {
public:
	SceneLoader() : lightDirTheta(0.785398), lightDirColor(0.2f, 0.2f, 0.2f) {}
	static const std::string tokens[];
	void loadScene(std::string scenePath);
	std::vector<std::string> modelPaths;
	std::vector<glm::bvec2> modelParams;
	std::vector<glm::vec3> modelPositions;
	std::vector<glm::vec3> modelScales;
	std::vector<glm::vec3> modelAngles;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightParams;
	std::vector<glm::vec3> lightColors;
	glm::vec3 lightDirColor;
	float lightDirTheta;
private:
	enum Tags {SCENE, MODEL, PATH, MODEL_PARAM, POSITION, 
			   SCALE, ANGLE, POINT_LIGHT, COLOR, POINT_PARAM, DIR_LIGHT, NUM_TAGS, NONE };
	void parseScene(const std::string& sceneInfo, size_t& index, enum Tags state);
	std::string parseTag(const std::string& sceneInfo, size_t& index);
	void parsePath(const std::string& sceneInfo, size_t& index, enum Tags previous);
	void parseParam(const std::string& sceneInfo, size_t& index, enum Tags previous);
	void parsePosition(const std::string& sceneInfo, size_t& index, enum Tags previous);
	void parseScale(const std::string& sceneInfo, size_t& index, enum Tags previous);
	void parseAngle(const std::string& sceneInfo, size_t& index, enum Tags previous);
	void parseColor(const std::string& sceneInfo, size_t& index, enum Tags previous);
};