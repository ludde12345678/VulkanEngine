# pragma once

#include <glm/glm.hpp>
#include "../Renderer/ShaderData.h"


struct InputState
{
	bool mouselook = false;

	bool W = false;
	bool A = false;
	bool S = false;
	bool D = false;

	bool LSHIFT = false;
	bool LCTR = false;
};



struct Camera {
	glm::vec3 position;
	float yaw;
	float pitch;
	glm::vec3 forward;
	glm::vec3 up = { 0, 0, -1 };
	float fov = 45.0f;

	float aspect = (16.0f / 9.0f);

	bool firstMouse = true;
	double lastX;
	double lastY;
	float moveSpeed = 0.02f;
};

struct RenderData
{
	UBOs Uniforms;
	Camera Camera;
	InputState inputState;
};
