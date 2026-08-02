#include "ResourceUploader.h"

void populateMesh(RendererContext& ctx, Mesh* mesh, std::vector<Vertex> vert, std::vector<uint32_t> indices) {
	if (vert.size() > 0) {
		mesh->vertexBuffer = createVertexBuffer<Vertex>(ctx.vulkanContext, vert);
	}
	mesh->vertexCount = vert.size();
	if (indices.size() > 0) {
		mesh->indexBuffer = createIndexBuffer<uint32_t>(ctx.vulkanContext, indices);
	}
	mesh->indexCount = indices.size();
}