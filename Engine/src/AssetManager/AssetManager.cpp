#pragma once

#include <Assets/AssetManager.h>
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
	for (const auto& textureData : jsonData["Assets"]["Textures"]) {
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
	std::unordered_map<std::string, MaterialHandle> materialMap;
	for (const auto& materialData : jsonData["Assets"]["Materials"]) {
		MaterialData material{};
		std::string materialName = materialData["Name"];
		if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading material: " << materialName << std::endl;
		}
		if (materialData.contains("AlbedoTexture") && materialData["AlbedoTexture"] != "") {
			material.albedoTexture = textureMap.at(materialData["AlbedoTexture"]);
		}
		if (materialData.contains("NormalTexture") && materialData["NormalTexture"] != "") {
			material.normalTexture = textureMap.at(materialData["NormalTexture"]);
		}
		if (materialData.contains("RoughnessTexture") && materialData["RoughnessTexture"] != "") {
			material.roughnessTexture = textureMap.at(materialData["RoughnessTexture"]);
		}
		if (materialData.contains("MetallicTexture") && materialData["MetallicTexture"] != "") {
			material.roughnessTexture = textureMap.at(materialData["MetallicTexture"]);
		}
		if (materialData.contains("AOTexture") && materialData["AOTexture"] != "") {
			material.roughnessTexture = textureMap.at(materialData["AOTexture"]);
		}
		if (materialData.contains("Color")) {
			material.baseColor = glm::vec4(materialData["Color"][0], materialData["Color"][1], materialData["Color"][2], materialData["Color"][3]);
		}
		if (materialData.contains("SpecularColor")) {
			material.baseColor = glm::vec4(materialData["SpecularColor"][0], materialData["SpecularColor"][1], materialData["SpecularColor"][2], materialData["SpecularColor"][3]);
		}
		if (materialData.contains("SpecularPower")) {
			material.specularPower = materialData["SpecularPower"];
		}
		if (materialData.contains("Metallic")) {
			material.specularPower = materialData["Metallic"];
		}
		if (materialData.contains("Roughness")) {
			material.specularPower = materialData["Roughness"];
		}
		if (materialData.contains("AO")) {
			material.specularPower = materialData["AO"];
		}
		MaterialHandle handle = renderer.loadMaterial(material);
		materialMap[materialData["Name"]] = handle;
	}
	// load meshes
	std::unordered_map<std::string, MeshHandle> meshMap;
	for (const auto& meshData : jsonData["Assets"]["Meshes"]) {
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
		// load components
		// transform
		if (objectData.contains("Transform") && objectData["Transform"].is_object()) {
			obj.transform =  Components::Transform{
			.position = glm::vec3(objectData["Transform"]["Position"][0], objectData["Transform"]["Position"][1], objectData["Transform"]["Position"][2]),
			.rotation = glm::vec3(objectData["Transform"]["Rotation"][0], objectData["Transform"]["Rotation"][1], objectData["Transform"]["Rotation"][2]),
			.scale = glm::vec3(objectData["Transform"]["Scale"][0], objectData["Transform"]["Scale"][1], objectData["Transform"]["Scale"][2]),
			};
		}
		else { throw std::runtime_error("Error: Scene parsing error, Object missing Transform"); }
		// mesh
		if (objectData.contains("Mesh") && objectData["Mesh"].is_object()) {
			Components::Mesh mesh{};
			mesh.mesh = meshMap.at(objectData["Mesh"]["Mesh"]);
			if (objectData["Mesh"].contains("Material") && objectData["Mesh"]["Material"].is_string()) {
				mesh.material = materialMap.at(objectData["Mesh"]["Material"]);
			}
			obj.mesh = mesh;

		}
		// light
		if (objectData.contains("Light") && objectData["Light"].is_object()) {
			obj.light = Components::Light{
				.type = Components::lightTypeFromString(objectData["Light"]["Type"]),
				.color = glm::vec3(objectData["Light"]["Color"][0], objectData["Light"]["Color"][1], objectData["Light"]["Color"][2]),
				.intensity = objectData["Light"]["Intensity"],
				.range = objectData["Light"]["Range"],
				.spotInnerAngle = objectData["Light"]["SpotInnerAngle"],
				.spotOuterAngle = objectData["Light"]["SpotOuterAngle"],
				.castsShadows = objectData["Light"]["ShadowCaster"],
				.shadowDistance = objectData["Light"]["ShadowDistance"]
			};
		}
		// camera
		if (objectData.contains("Camera") && objectData["Camera"].is_object()) {
			obj.camera = Components::Camera{
				.projection = Components::CameraProjectionFromString(objectData["Camera"]["Projection"]),
				.aspectRatio = objectData["Camera"]["AspectRatio"],
				.fov = objectData["Camera"]["FOV"],
				.nearPlane = objectData["Camera"]["NearPlane"],
				.farPlane = objectData["Camera"]["FarPlane"]
			};
		}
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