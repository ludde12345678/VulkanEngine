#include <iostream>
#include <exception>
#include <cstdlib>

#include <Renderer/Renderer.h>
#include <Assets/AssetManager.h>
#include <Window/Window.h>
#include <Types/Scene.h>
#include <Input/Input.h>
#include <Core/Time.h>
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
	Time time;
	float moveSpeed = 10.5f;
	float mouseSensitivity = 0.2f;
	bool drawDebugUI = true;
	// get camera
	ObjectHandle camera = scene.findObjectByName("Main Camera");
	ObjectHandle light = scene.findObjectByName("dir Light");
	ObjectHandle dirLight = scene.findObjectByName("dir Light");
	try {
		while (!window.shouldClose())
		{
			time.update();
			double dtime = time.dtime();
			input.update();
			window.pollWindow();

			if (input.wasPressed(Key::F1)) {
				std::cout << "F1 pressed\n";
				drawDebugUI = !drawDebugUI;
			}
			renderer.beginImGuiFrame(drawDebugUI);

			Object& cam = scene.getObject(camera);
			Object& l = scene.getObject(light);
			Object& ldir = scene.getObject(dirLight);
			glm::mat4 rotation = glm::yawPitchRoll(
				glm::radians(cam.transform.rotation.y),
				glm::radians(cam.transform.rotation.x),
				glm::radians(cam.transform.rotation.z)
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
			if (input.isDown(Key::LCTL)) {
				if (input.isDown(Key::W)) {
					l.transform.position += forward * moveSpeed * (float)dtime;
				}
				if (input.isDown(Key::S)) {
					l.transform.position -= forward * moveSpeed * (float)dtime;
				}
				if (input.isDown(Key::A)) {
					l.transform.position += right * moveSpeed * (float)dtime;
				}
				if (input.isDown(Key::D)) {
					l.transform.position -= right * moveSpeed * (float)dtime;
				}
			}
			else {
				if (input.isDown(Key::W)) {
					cam.transform.position += forward * moveSpeed * (float)dtime;
				}
				if (input.isDown(Key::S)) {
					cam.transform.position -= forward * moveSpeed * (float)dtime;
				}
				if (input.isDown(Key::A)) {
					cam.transform.position += right * moveSpeed * (float)dtime;
				}
				if (input.isDown(Key::D)) {
					cam.transform.position -= right * moveSpeed * (float)dtime;
				}
			}
			if (input.wasPressed(Key::RCLICK)) {
				window.disableCursor(true);
			}
			if (input.wasReleased(Key::RCLICK)) {
				window.disableCursor(false);
			}
			if (input.wasPressed(Key::ESC)) {
				window.close();
			}

			if (input.isDown(Key::RCLICK)) {
				glm::vec2 delta = input.mouseDelta();
				if (input.isDown(Key::LSHIFT)) {
					ldir.transform.rotation.x += delta.y * mouseSensitivity;
					ldir.transform.rotation.y -= delta.x * mouseSensitivity;
				}
				else {
					
					cam.transform.rotation.x += delta.y * mouseSensitivity;
					cam.transform.rotation.y -= delta.x * mouseSensitivity;

					cam.transform.rotation.x = glm::clamp(
						cam.transform.rotation.x,
						-89.0f,
						89.0f
					);
				}
			}

			//std::cout << time.FPS() << "\n";
			renderer.Render(scene);

		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

