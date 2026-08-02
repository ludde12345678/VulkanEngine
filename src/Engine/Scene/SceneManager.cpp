#include "SceneManager.h"

#include "../Renderer/Vulkan/MemoryHandler/memory.h"

Scene createDebugScene(RendererContext& ctx)
{
	Scene scene;
	loadDuplicateModels(
		ctx,
		"utah_teapot.obj",
		100,
		DefaultVertexLayouts::PositionNormalTexcoord(),
		scene
	);

	loadDuplicateModels(
		ctx,
		"simple_cube.obj",
		100,
		DefaultVertexLayouts::PositionNormalTexcoord(),
		scene
	);

	loadDuplicateModels(
		ctx,
		"spaceship.obj",
		100,
		DefaultVertexLayouts::PositionNormalTexcoord(),
		scene
	);

	// Teapots
	for (size_t i = 0; i < 100; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(i * 5.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(i * 8.0f),
			glm::vec3(0, 1, 0));

		scene.objects[i].transform = model;
	}

	// Bunnies
	for (size_t i = 100; i < 200; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3((i - 100) * 5.0f, 5.0f, 0.0f));

		scene.objects[i].transform = model;
	}

	// Ships
	for (size_t i = 200; i < 300; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3((i - 200) * 5.0f, 10.0f, 0.0f));

		scene.objects[i].transform = model;
	}


	return scene;


}

Scene createTriangleScene(AppContext& ctx)
{

	Mesh triangle{};
	Mesh triangle2{};
	std::vector<float> vertices =
	{
		// position          // color
		-0.75f, -0.433f, 0.0f,
		-0.25f, -0.433f, 0.0f,
		-0.50f,  0.433f, 0.0f,
	};
	std::vector<float> vertices2 =
	{
		// position          // color
		0.25f, -0.433f, 0.0f,   0.0f, 1.0f, 1.0f, 1.0f,
		0.75f, -0.433f, 0.0f,   0.0f, 1.0f, 1.0f, 1.0f,
		0.50f,  0.433f, 0.0f,   0.0f, 1.0f, 1.0f, 1.0f
	};
	triangle.layout = DefaultVertexLayouts::Position();
	triangle.vertexBuffer = createVertexBuffer<float>(ctx.renderer.vulkanContext, vertices);
	triangle.vertexCount = 3;

	triangle2.layout = DefaultVertexLayouts::PositionColor();
	triangle2.vertexBuffer = createVertexBuffer<float>(ctx.renderer.vulkanContext, vertices2);
	triangle2.vertexCount = 3;
	Scene testScene{};
	testScene.meshes.push_back(std::move(triangle));
	testScene.meshes.push_back(std::move(triangle2));
	
	Object obj1{};
	obj1.meshIndex = 0;
	testScene.objects.push_back(obj1);
	Object obj2{};
	obj2.meshIndex = 1;
	testScene.objects.push_back(obj2);


	return testScene;
}