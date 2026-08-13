#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../Scene/Scene.h"
#include "AssetConfig.h"
#include "AssetTypes.h"

MeshData loadOBJMeshData(std::string filename);