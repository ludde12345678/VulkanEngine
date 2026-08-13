#include "AssetLoader.h"
#include <nlohmann/json.hpp>


Scene loadScene(RendererContext& ctx, std::string SceneFilename) {
	Scene scene;
	// open json file
	std::ifstream file(AssetConfig::ASSET_DIR / "Scenes" / SceneFilename);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open scene file: " + SceneFilename);
	}
	nlohmann::json jsonData;
	file >> jsonData;
	file.close();
	std::string sceneName = jsonData["Name"];
	if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
		std::cout << "Loading scene: " << sceneName << std::endl;
	}
	
	// load thextures
	std::unordered_map<std::string, uint32_t> textureMap;
	for (const auto& textureData : jsonData["Textures"]) {
		std::string textureFilename = textureData["Path"];
		std::string textureName = textureData["Name"];
		if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading texture: " << textureName  << " From path: "<< textureFilename << std::endl;
		}
		TextureData texture = loadPNGTexture(textureFilename);
		Texture tex;
		populateTexture(ctx, &tex, texture);
		scene.textures.push_back(std::move(tex));
		textureMap[textureName] = scene.textures.size() - 1;

	}

	//load materials
	std::unordered_map<std::string, uint32_t> materialMap;
	for(const auto& materialData : jsonData["Materials"]) {
		Material material;
		std::string materialName = materialData["Name"];
		if(AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading material: " << materialName << std::endl;
		}
		if(materialData.contains("Albedo") && materialData["Albedo"] != "") {
			material.AlbedoIndex = textureMap[materialData["Albedo"]];
		}
		else {
			material.AlbedoIndex = -1;
		}
		if(materialData.contains("Normal") && materialData["Normal"] != "") {
			material.NormalIndex = textureMap[materialData["Normal"]];
		}
		else {
			material.NormalIndex = -1;
		}
		if(materialData.contains("Roughness") && materialData["Roughness"] != "") {
			material.RoughnessIndex = textureMap[materialData["Roughness"]];
		}
		else {
			material.RoughnessIndex = -1;
		}

		material.color = glm::vec4(materialData["Color"][0], materialData["Color"][1], materialData["Color"][2], materialData["Color"][3]);
		material.specularIntensity = materialData["SpecularPower"];
		scene.materials.push_back(std::move(material));
		materialMap[materialData["Name"]] = scene.materials.size() - 1;
	}
	// load meshes
	std::unordered_map<std::string, uint32_t> meshMap;
	for (const auto& meshData : jsonData["Meshes"]) {
		std::string meshFilename = meshData["Path"];
		std::string meshName = meshData["Name"];
		if (AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading mesh: " << meshName << std::endl;
		}
		Mesh mesh;
		MeshData data = loadOBJMeshData(meshFilename);
		populateMesh(ctx, &mesh, data.vertices, data.indices);
		mesh.layout = DefaultVertexLayouts::PositionNormalTexcoord();
		scene.meshes.push_back(std::move(mesh));
		meshMap[meshName] = scene.meshes.size() - 1;
	}

	// load objects
	for (const auto& objectData : jsonData["Objects"]) {
		Object obj;
		obj.name = objectData["Name"];
		if(AssetConfig::PRINT_ASSET_LOAD_INFO) {
			std::cout << "Loading object: " << obj.name << std::endl;
		}

		if (objectData.contains("Mesh") && objectData["Mesh"] != "") {
			obj.meshIndex = meshMap[objectData["Mesh"]];
		}
		else {
			obj.meshIndex = -1;
		}
		if (objectData.contains("Material") && objectData["Material"] != "") {
			obj.materialIndex = materialMap[objectData["Material"]];
		}
		else {
			obj.materialIndex = -1;
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
	if(AssetConfig::PRINT_ASSET_LOAD_INFO) {
		std::cout << "Done loading scene: " << sceneName << std::endl;
	}
	return scene;
}













Scene loadMeshScene(RendererContext& ctx, std::string objFilename) {
	Scene scene;
	loadModel(ctx, objFilename, DefaultVertexLayouts::PositionNormalTexcoord(), scene);
	return scene;
}

void loadDuplicateModels(RendererContext& ctx, std::string filename, uint32_t count, VertexLayout layout, Scene& scene) {

	std::cout << "AssetRoot:" << (AssetConfig::ASSET_DIR).string() << "\n";
	std::cout << "filePath:" << (AssetConfig::ASSET_DIR / "Models" / filename).string() << "\n";

	Mesh mesh;
	MeshData data = loadOBJMeshData(filename);
	populateMesh(ctx, &mesh, data.vertices, data.indices);
	mesh.layout = layout;
	scene.meshes.push_back(std::move(mesh));
	uint32_t meshIndex = scene.meshes.size() - 1;
	std::cout << "Done reading mesh" << "\n";

	for (size_t i = 0; i < count; i++)
	{
		Object obj;
		obj.meshIndex = meshIndex;
		
		scene.objects.push_back(std::move(obj));
	}
	std::cout << "Created:" << count << " Objects" << "\n";
}

void loadModel (RendererContext& ctx, std::string filename, VertexLayout layout, Scene& scene) {
 
	
	std::cout << "AssetRoot:" << (AssetConfig::ASSET_DIR).string() << "\n";
	std::cout << "filePath:" << (AssetConfig::ASSET_DIR / "Models\\" / filename).string() << "\n";

	Mesh mesh;
	MeshData data = loadOBJMeshData(filename);
	populateMesh(ctx, &mesh, data.vertices, data.indices);
	mesh.layout = layout;
	scene.meshes.push_back(std::move(mesh));
	uint32_t meshIndex = scene.meshes.size() - 1;


	Object obj;
	obj.meshIndex = meshIndex;
	scene.objects.push_back(obj);

	std::cout << "Done reading mesh" << "\n";
	
}

