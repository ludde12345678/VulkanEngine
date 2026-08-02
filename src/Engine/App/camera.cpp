#include "camera.h"


void updateCameraUniforms(RenderData& renderData) {
	Camera *cam = &renderData.Camera;
	CameraUBO* ubo = &renderData.Uniforms.camera;


	renderData.Uniforms.shading.camerapos = glm::vec4(cam->position, 1);
	//renderData.Uniforms.shading.lightpos = glm::vec4(cam->position, 1);

	ubo->projection = glm::perspective(cam->fov,cam->aspect, 0.1f, 1000.0f);
	
	ubo->view = glm::lookAt(
		cam->position,
		cam->position + cam->forward,
		cam->up
	);
	


}