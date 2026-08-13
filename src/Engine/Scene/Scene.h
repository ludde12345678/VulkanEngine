#pragma once

#include "Object.h"


struct Scene
{
	std::string name;
	std::vector<Object> objects;
	std::vector<Material> materials;

	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};