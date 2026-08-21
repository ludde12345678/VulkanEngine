#include <iostream>
#include <exception>
#include <cstdlib>

#include <Renderer.h>
#include <AssetManager.h>
#include <Window.h>
#include <Types/Scene.h>
#include <Input.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

int main()
{
	Window window(1000, 1000, "WAWA");
	Renderer renderer(window);
	AssetManager assets(renderer);
	Scene scene = assets.loadScene("testscene1.json");
	Input input;
	window.setupGLFWCallbacks(input);
	renderer.loadScene(scene);

	float moveSpeed = 0.05f;
	float mouseSensitivity = 0.002f;

	// get camera
	ObjectHandle camera = scene.findObjectByName("Main Camera");
	try {
		while (!window.shouldClose())
		{
			input.update();
			window.pollWindow();
			Object& cam = scene.getObject(camera);
			glm::mat4 rotation = glm::yawPitchRoll(
				cam.rotation.y,
				cam.rotation.x,
				cam.rotation.z
			);

			glm::vec3 forward = glm::normalize(
				glm::vec3(rotation * glm::vec4(0, 0, 1, 0))
			);

			glm::vec3 right = glm::normalize(
				glm::vec3(rotation * glm::vec4(1, 0, 0, 0))
			);

			glm::vec3 up = glm::normalize(
				glm::vec3(rotation * glm::vec4(0, 1, 0, 0))
			);
			if (input.isDown(Key::W)) {
				cam.position += forward * moveSpeed;
			}
			if (input.isDown(Key::S)) {
				cam.position -= forward * moveSpeed;
			}
			if (input.isDown(Key::A)) {
				cam.position += right * moveSpeed;
			}
			if (input.isDown(Key::D)) {
				cam.position -= right * moveSpeed;
			}
			if (input.wasPressed(Key::RCLICK)) {
				window.disableCursor(true);
			}
			if (input.wasReleased(Key::RCLICK)) {
				window.disableCursor(false);
			}

			if (input.isDown(Key::RCLICK)) {
				glm::vec2 delta = input.mouseDelta();
				cam.rotation.x += delta.y * mouseSensitivity;
				cam.rotation.y -= delta.x * mouseSensitivity;

				cam.rotation.x = glm::clamp(
					cam.rotation.x,
					glm::radians(-89.0f),
					glm::radians(89.0f)
				);
			}


			renderer.Render(scene);

		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

