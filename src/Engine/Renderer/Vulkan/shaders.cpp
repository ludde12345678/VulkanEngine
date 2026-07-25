#include "shaders.h"

void createShaders(VulkanContext& ctx)
{

	auto vertCode = readFile((VConfig::SHADER_DIR / "main.vert.spv").string());
	auto fragCode = readFile((VConfig::SHADER_DIR / "main.frag.spv").string());
	
	std::vector<VkShaderCreateInfoEXT> createInfos{
	createShaderInfo(vertCode, VK_SHADER_STAGE_VERTEX_BIT),
	createShaderInfo(fragCode , VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	auto vkCreateShadersEXT = (PFN_vkCreateShadersEXT)vkGetDeviceProcAddr(ctx.device, "vkCreateShadersEXT");
	if (!vkCreateShadersEXT)
	{
		throw std::runtime_error("Shader Extension not loaded!");
	}

	std::vector<VkShaderEXT> shaderHandles(static_cast<uint32_t>(createInfos.size()));
	if (vkCreateShadersEXT(ctx.device, static_cast<uint32_t>(createInfos.size()),
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
	ctx.shaderContext = shaderCtx;

	

}

VkShaderCreateInfoEXT createShaderInfo(const std::vector<char>& code, VkShaderStageFlagBits stage) {
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

void destroyShaders(VulkanContext& ctx)
{
	auto vkDestroyShaderEXT =
		(PFN_vkDestroyShaderEXT)
		vkGetDeviceProcAddr(ctx.device, "vkDestroyShaderEXT");
	if (vkDestroyShaderEXT)
	{
		vkDestroyShaderEXT(ctx.device, ctx.shaderContext.vertexShader, nullptr);
		vkDestroyShaderEXT(ctx.device, ctx.shaderContext.fragmentShader, nullptr);
	}	


}


void createPipelineLayout(VulkanContext& ctx) {


	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	// descriptors
	layoutCreateInfo.setLayoutCount = 0;
	layoutCreateInfo.pSetLayouts = nullptr;

	//push constants
	layoutCreateInfo.pushConstantRangeCount = 0;
	layoutCreateInfo.pPushConstantRanges = nullptr;


	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &layout) != VK_SUCCESS) { throw std::runtime_error("Error: Pipeline layout creation failed"); }
	
	PipelineContext pipeCtx{};
	pipeCtx.pipelineLayout = layout;

	ctx.pipelineContext = pipeCtx;

}
void destroyPipelineLayout(VulkanContext& ctx) {

	vkDestroyPipelineLayout(ctx.device, ctx.pipelineContext.pipelineLayout, nullptr);

}
