#include "SceneLoader.h"

const std::string SceneLoader::tokens[] = {"scene", "model", "path", "model_param", "position", "scale", "angle", "point_light",
										   "color", "point_param", "dir_light"};

void SceneLoader::loadScene(std::string scenePath)
{
	std::ifstream sceneFile;
	sceneFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::string sceneInfo;
	try {
		std::ifstream sceneFile;
		std::stringstream sceneFileStream;
		sceneFile.open(scenePath);
		sceneFileStream << sceneFile.rdbuf();
		sceneFile.close();
		sceneInfo = sceneFileStream.str();
	}
	catch (std::ifstream::failure) {
		std::cout << "File: " << scenePath << " could not be read\n";
		return;
	}
	sceneInfo.erase(std::remove_if(sceneInfo.begin(), sceneInfo.end(), ::isspace), sceneInfo.end());
	size_t curIndex = 0;
	enum Tags currentState = NONE;
	while (curIndex < sceneInfo.size()) {
		size_t beginToken, endToken;
		curIndex = sceneInfo.find('<', curIndex);
	}
}

std::string SceneLoader::parseTag(std::string& sceneInfo, size_t& index) {
	return "";
}

void SceneLoader::parse(std::string& sceneInfo, size_t& index, Tags nextState) {

}
