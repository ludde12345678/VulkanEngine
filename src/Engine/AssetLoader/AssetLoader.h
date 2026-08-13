#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../Renderer/ResourceUploader.h"
#include "../Scene/Scene.h"
#include "AssetConfig.h"
#include "AssetTypes.h"
#include "MeshLoader.h"
#include "TextureLoader.h"

Scene loadScene(RendererContext &ctx, std::string SceneFilename);

Scene loadMeshScene(RendererContext &ctx, std::string objFilename);

void loadDuplicateModels(RendererContext& ctx, std::string filename, uint32_t count, VertexLayout layout, Scene& scene);

void loadModel(RendererContext& ctx, std::string filename, VertexLayout layout, Scene& scene);


