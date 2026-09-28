#include "Shaders.h"

void createShaders(RendererContext& ctx)
{
	if (!(std::filesystem::exists(VConfig::SHADER_DIR) && std::filesystem::is_directory(VConfig::SHADER_DIR))) { throw std::runtime_error("Shader folder not found"); }
	for (const auto& file : std::filesystem::directory_iterator(VConfig::SHADER_DIR)) {
		if (!std::filesystem::is_regular_file(file.status())) { continue; }
		
		std::cout << "Shader found at:" << (file.path().string()) << "\n";
		std::string shaderName = file.path().stem().stem().string();
		std::string extension = file.path().stem().extension().string();
		auto Code = readFile(file.path().string());
		VkShaderCreateInfoEXT createInfo;
		if (file.path().stem().extension().string() == ".frag") {
			createInfo = createShaderInfo(ctx, Code, shaderName, VK_SHADER_STAGE_FRAGMENT_BIT, 0);
		}
		else if (file.path().stem().extension().string() == ".vert") {
			createInfo = createShaderInfo(ctx, Code, shaderName, VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT);
		}
		else if (file.path().stem().extension().string() == ".compute") {
			createInfo = createShaderInfo(ctx, Code, shaderName, VK_SHADER_STAGE_COMPUTE_BIT, 0);
		}
		else {
			continue;
		}

		if (!vkCreateShadersEXT){throw std::runtime_error("Shader Extension not loaded!");}

		VkShaderEXT shaderHandle;
		VK_CHECK(vkCreateShadersEXT(
			ctx.vulkanContext.device,
			1,
			&createInfo,
			nullptr,
			&shaderHandle
		
		), "Error: Shader creation failed");

		if (VConfig::PRINT_DEBUG_INFO) {
			std::cout << "Shader Handle: " << shaderHandle << "\n";
		}

		// TODO: Will leak if duplicate is discovered, since its already created the shader, fix if u have the time
		if (extension == ".frag") {
			if (ctx.shaders.fragmentShaders.contains(shaderName)) {
				throw std::runtime_error("Duplicate fragment shader name: " + shaderName);
			}
			ctx.shaders.fragmentShaders[shaderName] = shaderHandle;
		}
		else if (extension == ".vert") {
			if (ctx.shaders.vertexShaders.contains(shaderName)) {
				throw std::runtime_error("Duplicate vertex shader name: " + shaderName);
			}
			ctx.shaders.vertexShaders[shaderName] = shaderHandle;
		}
		else if (extension == ".compute") {
			if (ctx.shaders.computeShaders.contains(shaderName)) {
				throw std::runtime_error("Duplicate compute shader name: " + shaderName);
			}
			ctx.shaders.computeShaders[shaderName] = shaderHandle;
		}
		
	}
	
}

VkShaderCreateInfoEXT createShaderInfo(RendererContext& ctx, const std::vector<char>& code,std::string& name, VkShaderStageFlagBits stage, VkShaderStageFlags nextStage) {
	VkShaderCreateInfoEXT shaderInfo{};
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
	shaderInfo.stage = stage;
	shaderInfo.nextStage = nextStage;

	

	shaderInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
	
	shaderInfo.codeSize = code.size();
	shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	shaderInfo.pName = "main";
	shaderInfo.setLayoutCount = ctx.Descriptors.layouts.size();
	shaderInfo.pSetLayouts = ctx.Descriptors.layouts.data();
	shaderInfo.pushConstantRangeCount = ctx.Descriptors.pushConstants.size();
	shaderInfo.pPushConstantRanges = ctx.Descriptors.pushConstants.data();
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
	for (auto& [name, shader] : ctx.shaders.vertexShaders) {
		vkDestroyShaderEXT(ctx.vulkanContext.device, shader, nullptr);
	}
	for (auto& [name, shader] : ctx.shaders.fragmentShaders) {
		vkDestroyShaderEXT(ctx.vulkanContext.device, shader, nullptr);
	}
	for (auto& [name, shader] : ctx.shaders.computeShaders) {
		vkDestroyShaderEXT(ctx.vulkanContext.device, shader, nullptr);
	}
}

void bindShaders(RendererContext& ctx, VkCommandBuffer cmd, const std::string& name) {
	std::vector<VkShaderStageFlagBits> stages;
	stages.reserve(2);
	std::vector<VkShaderEXT> shaders;
	shaders.reserve(2);
	if (ctx.shaders.vertexShaders.contains(name)) {
		stages.push_back(VK_SHADER_STAGE_VERTEX_BIT);
		shaders.push_back(ctx.shaders.vertexShaders.at(name));
	}
	if (ctx.shaders.fragmentShaders.contains(name)) {
		stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
		shaders.push_back(ctx.shaders.fragmentShaders.at(name));
	}
	else {
		// No fragment shader: explicitly disable the fragment stage.
		stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
		shaders.push_back(VK_NULL_HANDLE);
	}
	if (!ctx.shaders.vertexShaders.contains(name)) {
		throw std::runtime_error("No vertex shader found: " + name);
	}
	vkCmdBindShadersEXT(cmd, stages.size(), stages.data(), shaders.data());

}

