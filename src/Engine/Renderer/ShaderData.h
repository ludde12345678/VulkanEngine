#pragma once

#include <volk.h>
#include <iostream>
#include <fstream>
#include <string>



struct TimeUBO {
	uint32_t FrameCount = 0;
};

struct CameraUBO
{
	glm::mat4 view = glm::mat4(1);
    glm::mat4 projection = glm::mat4(1);
};
struct ShadingUBO
{
	glm::vec4 diffuseColor = glm::vec4(0.6, 0.6, 0.6, 1);
	glm::vec4 spectralColor = glm::vec4(1, 1, 1, 1);
	glm::vec4 ambientColor = glm::vec4(0.1, 0.1, 0.1, 1);

	glm::vec4 lightpos = glm::vec4(0, 15, 2, 1);
	glm::vec4 camerapos = glm::vec4(0, -5, 5, 1);

	float spectralPower = 40.0f;

	// toggles
	uint32_t toggleDiffuse = 1;
	uint32_t toggleSpecular = 1;
	uint32_t toggleAmbient = 1;

	uint32_t debugNormals = 0;
};




struct UBOs {
	TimeUBO time;
	CameraUBO camera;
	ShadingUBO shading;
};