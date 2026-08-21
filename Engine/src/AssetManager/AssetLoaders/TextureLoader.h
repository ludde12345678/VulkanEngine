#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../AssetConfig.h"
#include "../AssetTypes.h"
#include "BinaryReader.h"

TextureData loadPNGTexture(std::string filename);