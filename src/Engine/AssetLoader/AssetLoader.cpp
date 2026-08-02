#include "AssetLoader.h"


Scene loadglTFScene(RendererContext& ctx, std::string objFilename) {
	Scene scene;


	
	return scene;
}


Scene loadMeshScene(RendererContext& ctx, std::string objFilename) {
	Scene scene;
	loadModel(ctx, objFilename, DefaultVertexLayouts::PositionNormalTexcoord(), scene);
	return scene;
}

void loadDuplicateModels(RendererContext& ctx, std::string filename, uint32_t count, VertexLayout layout, Scene& scene) {

	std::cout << "AssetRoot:" << (AssetConfig::ASSET_DIR).string() << "\n";
	std::cout << "filePath:" << (AssetConfig::ASSET_DIR / "Models\\" / filename).string() << "\n";

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
	obj.transform = glm::mat4(1.0f);
	scene.objects.push_back(obj);

	std::cout << "Done reading mesh" << "\n";
	
}


MeshData loadOBJMeshData(std::string filename) {
	std::ifstream file((AssetConfig::ASSET_DIR / "Models\\" / filename).string());

	if (!file) { throw std::runtime_error("Error reading obj file"); }
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<VertexKey, uint32_t, VertexKeyHash> map;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	file.seekg(0);
	std::string text;
	while (std::getline(file, text))
	{
		std::stringstream ss(text);

		std::vector<std::string> tokens;
		std::string token;

		while (ss >> token)
		{
			tokens.push_back(token);
		}
		if (tokens.empty()) { continue; }

		//std::cout << "[" << text << "] -> " << tokens.size() << '\n';
		if (tokens[0] == "#") {
			// Comment, ignore
			continue;
		}
		else if (tokens[0] == "v") {
			// vertex
			if (tokens.size() >= 4)
			{
				positions.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));

			}
		}
		else if (tokens[0] == "vn") {
			// normals
			if (tokens.size() >= 4)
			{
				normals.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
			}
		}
		else if (tokens[0] == "vt") {
			// texture coords
			if (tokens.size() >= 3)
			{
				texcoords.push_back(glm::vec2(std::stof(tokens[1]), std::stof(tokens[2])));

			}
		}
		else if (tokens[0] == "f") {
			// Polygonal face element??
			for (size_t i = 0; i < 3; i++)
			{
				std::vector<uint32_t> ints;
				std::string temp;
				for (char character : tokens[i + 1]) {
					if (character != '/') {
						temp += character;
					}
					else
					{
						ints.push_back(std::stoi(temp));
						temp = "";
					}
				}
				ints.push_back(std::stof(temp));

				VertexKey key{
				ints[0] - 1,
				ints[1] - 1,
				ints[2] - 1
				};


				auto it = map.find(key);
				if (it != map.end())
				{
					// Vertex already exists
					indices.push_back(it->second);
				}
				else {

					Vertex v;
					v.position = positions[key.pos];
					v.texCoord = texcoords[key.uv];
					v.normal = normals[key.normal];

					uint32_t index = static_cast<uint32_t>(vertices.size());

					vertices.push_back(v);
					indices.push_back(index);

					map[key] = index;

				}
			}
		}
	}
	MeshData data;
	data.indices = indices;
	data.vertices = vertices;

	file.close();
	return data;
}