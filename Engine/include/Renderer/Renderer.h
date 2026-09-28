#pragma once
#include <Types/Scene.h>
#include <Window/Window.h>
#include <memory>
#include <Renderer/RenderSettings.h>


struct TextureData;
struct MeshData;
struct MaterialData;

class Renderer {

public:
	Renderer(Window& window);
	~Renderer();
	void Render(Scene& scene);
	void setCamera(std::string name);
	void loadScene(Scene& scene);
	void unloadScene();
	void beginImGuiFrame(bool drawEngineDebug = false);
	Settings& getRenderSettings();

private:
	void initImGui();
	void CreateEngineUI();
	void ImGuiCheckbox(const char* label, uint32_t& value);
	void printRendererDebugInfo();
	void initLightSSBO();
	void updatePerFrameData();
	void findFallbackCamera();
	// Asset Interface Friends
	friend class AssetManager;
	TextureHandle loadTexture(TextureData& texture);
	MeshHandle loadMesh(MeshData& mesh);
	MaterialHandle loadMaterial(MaterialData& matData);

	//void unloadTexture(TextureHandle);
	//void unloadMesh(MeshHandle);

	struct Impl;
	std::unique_ptr<Impl> vulkan;

};