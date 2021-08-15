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
	std::vector<glm::vec3> modelPositions;
	std::vector<glm::vec3> modelScales;
	std::vector<glm::vec3> modelAngles;
	std::vector<glm::vec3> lightPos;
	std::vector<glm::vec3> lightParam;
	std::vector<glm::vec3> lightColor;
	glm::vec3 lightDirColor;
	float lightDirTheta;
private:
	enum Tags {SCENE, MODEL, PATH, MODEL_PARAM, POSITION, 
			   SCALE, ANGLE, POINT_LIGHT, COLOR, POINT_PARAM, DIR_LIGHT, NUM_TAGS, NONE };
	void parse(std::string& sceneInfo, size_t& index, enum Tags nextState);
	std::string parseTag(std::string& sceneInfo, size_t& index);

};