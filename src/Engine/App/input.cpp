#include "input.h"

void updateInputs(RenderData& renderData)
{

	Camera* cam = &renderData.Camera;
	glm::vec3 right = glm::normalize(glm::cross(cam->forward, cam->up));
	if (renderData.inputState.W) {
		cam->position += (cam->forward * cam->moveSpeed);
	}
	if (renderData.inputState.S) {
		cam->position -= (cam->forward * cam->moveSpeed);
	}
	if (renderData.inputState.A) {
		cam->position -= (right * cam->moveSpeed);
	}
	if (renderData.inputState.D) {
		cam->position += (right * cam->moveSpeed);
	}

	if (renderData.inputState.LSHIFT) {
		cam->position -= (cam->up * cam->moveSpeed);
	}
	if (renderData.inputState.LCTR) {
		cam->position += (cam->up * cam->moveSpeed);
	}

}
