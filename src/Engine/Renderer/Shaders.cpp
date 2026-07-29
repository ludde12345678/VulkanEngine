#include "Shaders.h"

void createShaders(RendererContext& ctx)
{

	auto vertCode = readFile((VConfig::SHADER_DIR / "main.vert.spv").string());
	auto fragCode = readFile((VConfig::SHADER_DIR / "main.frag.spv").string());
	
	std::vector<VkShaderCreateInfoEXT> createInfos{
	createShaderInfo(ctx, vertCode, VK_SHADER_STAGE_VERTEX_BIT),
	createShaderInfo(ctx, fragCode , VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	
	if (!vkCreateShadersEXT)
	{
		throw std::runtime_error("Shader Extension not loaded!");
	}

	std::vector<VkShaderEXT> shaderHandles(static_cast<uint32_t>(createInfos.size()));
	if (vkCreateShadersEXT(ctx.vulkanContext.device, static_cast<uint32_t>(createInfos.size()),
		createInfos.data(), nullptr, shaderHandles.data()) != VK_SUCCESS) {
		throw std::runtime_error("Error: Shader creation failed");
	}

	if (VConfig::PRINT_DEBUG_INFO) {
		std::cout << "Vertex shader: " << shaderHandles[0] << "\n";
		std::cout << "Fragment shader: " << shaderHandles[1] << "\n";
	}

	ShaderContext shaderCtx{
	shaderHandles[0],
	shaderHandles[1]
	};
	ctx.shaders = shaderCtx;

	

}

VkShaderCreateInfoEXT createShaderInfo(RendererContext& ctx, const std::vector<char>& code, VkShaderStageFlagBits stage) {
	VkShaderCreateInfoEXT shaderInfo{};
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
	shaderInfo.stage = stage;
	if (stage == VK_SHADER_STAGE_VERTEX_BIT) {
		shaderInfo.nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	

	shaderInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
	
	shaderInfo.codeSize = code.size();
	shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	shaderInfo.pName = "main";
	shaderInfo.setLayoutCount = 1;
	shaderInfo.pSetLayouts = &ctx.descriptors.timeLayout;
	shaderInfo.pNext = nullptr;

	return shaderInfo;
}

std::vector<char> readFile(const std::string filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	
	if (!file) { throw std::runtime_error("Error reading shader file"); }
	size_t size = file.tellg();
	std::vector<char> buf(size);


	file.seekg(0);
	file.read(buf.data(), size);

	file.close();
	return buf;
}

void destroyShaders(RendererContext& ctx)
{
	if (vkDestroyShaderEXT)
	{
		vkDestroyShaderEXT(ctx.vulkanContext.device, ctx.shaders.vertexShader, nullptr);
		vkDestroyShaderEXT(ctx.vulkanContext.device, ctx.shaders.fragmentShader, nullptr);
	}	


}

void bindShaders(RendererContext& ctx, VkCommandBuffer cmd) {
	static const VkShaderStageFlagBits stages[] = {
	VK_SHADER_STAGE_VERTEX_BIT,
	VK_SHADER_STAGE_FRAGMENT_BIT
	};
	static const VkShaderEXT shaders[] = {
	ctx.shaders.vertexShader,
	ctx.shaders.fragmentShader

	};

	vkCmdBindShadersEXT(cmd, static_cast<uint32_t>(std::size(stages)), stages, shaders);

}

