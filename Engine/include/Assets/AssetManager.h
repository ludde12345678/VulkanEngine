#pragma once
#include <vector>
#include <Types/Scene.h>
#include <Renderer/Renderer.h>
#include <string>


class AssetManager {
public:
	AssetManager(Renderer& renderer);
	Scene loadScene(std::string filename );
	//std::vector<Scene> loadScenes(std::vector<std::string> filenames);

	void unloadScene(std::string filename);
	//void unloadScenes(std::vector<std::string> filenames);

	~AssetManager();
private:
	Renderer& renderer;
};