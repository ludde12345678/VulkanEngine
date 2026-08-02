#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../Renderer/ResourceUploader.h"
#include "../Scene/Scene.h"
#include "AssetConfig.h"

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct VertexKey
{
    uint32_t pos;
    uint32_t uv;
    uint32_t normal;

    bool operator==(const VertexKey&) const = default;
};
struct VertexKeyHash
{
    size_t operator()(const VertexKey& key) const
    {
        size_t h1 = std::hash<uint32_t>{}(key.pos);
        size_t h2 = std::hash<uint32_t>{}(key.normal);
        size_t h3 = std::hash<uint32_t>{}(key.uv);

        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

Scene loadMeshScene(RendererContext& ctx, std::string objFilename);

void loadDuplicateModels(RendererContext& ctx, std::string filename, uint32_t count, VertexLayout layout, Scene& scene);

void loadModel(RendererContext& ctx, std::string filename, VertexLayout layout, Scene& scene);

MeshData loadOBJMeshData(std::string filename);
