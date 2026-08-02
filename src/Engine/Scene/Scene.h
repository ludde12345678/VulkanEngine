#pragma once

#include "Object.h"

struct Scene
{
	std::vector<Object> objects;
	std::vector<Mesh> meshes;
};