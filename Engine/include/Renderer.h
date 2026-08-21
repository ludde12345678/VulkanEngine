#pragma once
#include <Types/Scene.h>
#include <Window.h>
#include <memory>


struct TextureData;
struct MeshData;

class Renderer {

public:
	Renderer(Window& window);
	~Renderer();
	void Render(Scene& scene);
	void setCamera(std::string name);
	void loadScene(Scene& scene);
	void unloadScene(Scene& scene);

private:
	void initImGui();
	void CreateEngineUI();
	void ImGuiCheckbox(const char* label, uint32_t& value);
	void printRendererDebugInfo();
	void updateCameraUniforms();
	void findFallbackCamera();
	// Asset Interface Friends
	friend class AssetManager;
	TextureHandle loadTexture(TextureData& texture);
	MeshHandle loadMesh(MeshData& mesh);

	//void unloadTexture(TextureHandle);
	//void unloadMesh(MeshHandle);

	struct Impl;
	std::unique_ptr<Impl> vulkan;

};