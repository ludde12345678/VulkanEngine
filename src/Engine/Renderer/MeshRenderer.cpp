#include "MeshRenderer.h"

void drawMesh(VkCommandBuffer cmd, const Mesh& mesh)
{
	// vertex inputs
	SetAndbindVertexInputs(cmd, mesh);
	if (mesh.indexCount > 0) {
		vkCmdBindIndexBuffer(
			cmd,
			mesh.indexBuffer.buffer,
			0,
			mesh.indexType
		);
		vkCmdDrawIndexed(
			cmd,
			mesh.indexCount,
			1,
			0,
			0,
			0
		);
	}
	else
	{
		vkCmdDraw(cmd, 
			mesh.vertexCount,
			1,
			0,
			0);
	}


}

void SetAndbindVertexInputs(VkCommandBuffer cmd, const Mesh& mesh) {

	vkCmdSetVertexInputEXT(
		cmd,
		static_cast<uint32_t>(mesh.layout.bindings.size()),
		mesh.layout.bindings.data(),
		static_cast<uint32_t>(mesh.layout.attributes.size()),
		mesh.layout.attributes.data()
	);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers2EXT(
		cmd,
		0,
		1,
		&mesh.vertexBuffer.buffer,
		&offset,
		nullptr,
		nullptr
	);
}
