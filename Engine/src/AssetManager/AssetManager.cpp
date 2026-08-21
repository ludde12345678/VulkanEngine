#include <AssetManager.h>
#include <string>
#include <vector>
#include "AssetTypes.h"
#include "AssetLoaders/TextureLoader.h"
#include "AssetLoaders/MeshLoader.h"
#include <nlohmann/json.hpp>
#include "AssetConfig.h"

AssetManager::AssetManager(Renderer& renderer)
: renderer(renderer)
{

}




Scene AssetManager::loadScene(std::string filename) {
	Scene scene;
	// open json file
	std::ifstream file(AssetConfig::ASSET_DIR / "Scenes" / filename);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open scene file: " + filename);
	}
	nlohmann::json jsonData;
	file >> jsonData;
	file.close();
	std::string sceneName = jsonData["Name"];
	if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
		std::cout << "Loading scene: " << sceneName << std::endl;
	}

	// load thextures
	std::unordered_map<std::string, TextureHandle> textureMap;
	for (const auto& textureData : jsonData["Textures"]) {
		std::string textureFilename = (AssetConfig::ASSET_DIR / textureData["Path"]).string();
		std::string textureName = textureData["Name"];
		if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading texture: " << textureName << " From path: " << textureFilename << std::endl;
		}
		TextureData texture = loadPNGTexture(textureFilename);
		TextureHandle handle = renderer.loadTexture(texture);
		textureMap[textureName] = handle;

	}

	//load materials
	std::unordered_map<std::string, Material> materialMap;
	for (const auto& materialData : jsonData["Materials"]) {
		Material material{};
		std::string materialName = materialData["Name"];
		if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading material: " << materialName << std::endl;
		}
		if (materialData.contains("Albedo") && materialData["Albedo"] != "") {
			material.AlbedoIndex = textureMap[materialData["Albedo"]];
		}
		if (materialData.contains("Normal") && materialData["Normal"] != "") {
			material.NormalIndex = textureMap[materialData["Normal"]];
		}
		if (materialData.contains("Roughness") && materialData["Roughness"] != "") {
			material.RoughnessIndex = textureMap[materialData["Roughness"]];
		}
		if (materialData.contains("Color")) {
			material.color = glm::vec4(materialData["Color"][0], materialData["Color"][1], materialData["Color"][2], materialData["Color"][3]);
		}
		if (materialData.contains("SpecularPower")) {
			material.specularIntensity = materialData["SpecularPower"];
		}
		materialMap[materialData["Name"]] = material;
	}
	// load meshes
	std::unordered_map<std::string, MeshHandle> meshMap;
	for (const auto& meshData : jsonData["Meshes"]) {
		std::string meshFilename = (AssetConfig::ASSET_DIR / meshData["Path"]).string();
		std::string meshName = meshData["Name"];
		if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading mesh: " << meshName << std::endl;
		}
		MeshData data = loadOBJMeshData(meshFilename);
		MeshHandle meshHandle = renderer.loadMesh(data);
		
		meshMap[meshName] = meshHandle;
	}

	// load objects
	for (const auto& objectData : jsonData["Objects"]) {
		Object obj;
		obj.name = objectData["Name"];
		if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading object: " << obj.name << std::endl;
		}

		if (objectData.contains("Mesh") && objectData["Mesh"] != "") {
			obj.mesh = meshMap[objectData["Mesh"]];
		}
		else {
			obj.mesh = MeshHandle{};
		}
		if (objectData.contains("Material") && objectData["Material"] != "") {
			obj.material = materialMap[objectData["Material"]];
		}
		else {
			obj.material = Material{};
		}

		for (const auto& type : objectData["Type"]) {
			if (type == "Mesh") {
				obj.components.push_back(ObjectComponents::Mesh);
			}
			else if (type == "Material") {
				obj.components.push_back(ObjectComponents::Material);
			}
			else if (type == "PointLight") {
				obj.components.push_back(ObjectComponents::PointLight);
			}
			else if (type == "Camera") {
				obj.components.push_back(ObjectComponents::Camera);
			}
		}

		obj.position = glm::vec3(objectData["Position"][0], objectData["Position"][1], objectData["Position"][2]);
		obj.rotation = glm::vec3(objectData["Rotation"][0], objectData["Rotation"][1], objectData["Rotation"][2]);
		obj.scale = glm::vec3(objectData["Scale"][0], objectData["Scale"][1], objectData["Scale"][2]);
		scene.objects.push_back(std::move(obj));

	}
	// global data

	scene.name = sceneName;
	if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
		std::cout << "Done loading scene: " << sceneName << std::endl;
	}
	return scene;

}
//std::vector<Scene> AssetManager::loadScenes(std::vector<std::string> filenames) {

//}

void AssetManager::unloadScene(std::string filename) {

}
//void AssetManager::unloadScenes(std::vector<std::string> filenames) {

//}





AssetManager::~AssetManager() {

}