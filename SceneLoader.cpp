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
	size_t index = 0;
	std::string token = parseTag(sceneInfo, index);
	if (token == tokens[SCENE]) parseScene(sceneInfo, index, SCENE);
	else std::cout << "Invalid token: " << token << ". Scene incomplete.\n";
}

std::string SceneLoader::parseTag(const std::string& sceneInfo, size_t& index) {
	size_t beginToken = sceneInfo.find('<', index);
	beginToken++;
	size_t endToken = sceneInfo.find('>', beginToken);
	index = endToken + 1;
	endToken--;
	std::string result = sceneInfo.substr(beginToken, endToken - beginToken + 1);
	return result;
}

void SceneLoader::parseParam(const std::string& sceneInfo, size_t& index, enum Tags previous)
{
	if (previous == MODEL) {
		glm::bvec2 modelParam(true, false); //FLIP, NO_SINGLE
		size_t beginToken = index;
		size_t endToken = sceneInfo.find('<', index);
		endToken--;
		index = endToken;
		std::string params = sceneInfo.substr(beginToken, endToken - beginToken + 1);
		endToken = sceneInfo.find(',', beginToken);
		if (endToken == std::string::npos) {
			std::cout << "Invalid syntax\n";
			return;
		}
		std::string param1 = sceneInfo.substr(beginToken, endToken);
		if (param1 == "FLIP") modelParam.x = true;
		else if (param1 == "NO_FLIP") modelParam.x = false;
		else {
			std::cout << "Invalid token: " << param1 << std::endl;
			return;
		}
		beginToken = endToken + 1;
		endToken = sceneInfo.find(',', beginToken);
		if (endToken == std::string::npos) {
			std::cout << "Invalid syntax\n";
			return;
		}
		std::string param2 = sceneInfo.substr(beginToken, endToken);
		if (param1 == "SINGLE") modelParam.y = true;
		else if (param1 == "NO_SINGLE") modelParam.y = false;
		else {
			std::cout << "Invalid token: " << param2 << std::endl;
			return;
		}
		modelParams.push_back(modelParam);
		parseTag(sceneInfo, index);
	}
	else if (previous == POINT_LIGHT) {
		glm::vec3 lightParam;
		size_t beginToken = index;
		size_t endToken = sceneInfo.find('<', index);
		endToken--;
		index = endToken;
		std::string params = sceneInfo.substr(beginToken, endToken - beginToken + 1);
		endToken = sceneInfo.find(',', beginToken);
		if (endToken == std::string::npos) {
			std::cout << "Invalid syntax\n";
			return;
		}
		std::string param1 = sceneInfo.substr(beginToken, endToken);
		lightParam.x = std::stof(param1);
		beginToken = endToken + 1;
		endToken = sceneInfo.find(',', beginToken);
		if (endToken == std::string::npos) {
			std::cout << "Invalid syntax\n";
			return;
		}
		std::string param2 = sceneInfo.substr(beginToken, endToken);
		lightParam.y = std::stof(param2);
		beginToken = endToken + 1;
		endToken = sceneInfo.find(',', beginToken);
		if (endToken == std::string::npos) {
			std::cout << "Invalid syntax\n";
			return;
		}
		std::string param3 = sceneInfo.substr(beginToken, endToken);
		lightParam.z = std::stof(param3);
		parseTag(sceneInfo, index);
	}
	else {
		std::cout << "Invalid token: params, given previous token: " << tokens[previous] << std::endl;
	}
}

void SceneLoader::parsePath(const std::string& sceneInfo, size_t& index, enum Tags previous)
{
	if (previous == MODEL) {
		size_t beginToken = index;
		size_t endToken = sceneInfo.find('<', index);
		endToken--;
		index = endToken;
		modelPaths.push_back(sceneInfo.substr(beginToken, endToken - beginToken + 1));
	}
	else {
		std::cout << "Invalid token: " <<
			tokens[PATH] << ", given previous token: " << tokens[previous] << std::endl;
	}
}

void SceneLoader::parseScene(const std::string& sceneInfo, size_t& index, enum Tags state) {
	std::string token;
	while (token != ("/" + tokens[state]) && index < sceneInfo.size()) {
		std::string token = parseTag(sceneInfo, index);
		if (token == tokens[MODEL]) {
			parseScene(sceneInfo, index, MODEL);
		} else if (token == tokens[POINT_LIGHT]) {
			parseScene(sceneInfo, index, POINT_LIGHT);
		} else if (token == tokens[DIR_LIGHT]) {
			parseScene(sceneInfo, index, DIR_LIGHT);
		} else if (token == tokens[PATH]) {
			parsePath(sceneInfo, index, state);
		} else if (token == tokens[MODEL_PARAM] || token == tokens[POINT_PARAM]) {
			parseParam(sceneInfo, index, state);
		} else if (token == tokens[SCALE]) {
			parseScale(sceneInfo, index, state);
		} else if (token == tokens[ANGLE]) {
			parseAngle(sceneInfo, index, state);
		} else if (token == tokens[COLOR]) {
			parseColor(sceneInfo, index, state);
		} else {
			std::cout << "Invalid token: " << token << ". Scene incomplete.\n";
		}
	}
}
