#pragma once
#include <array>
#include <glm/glm.hpp>

struct GLFWwindow;

enum class Key {
	W,
	A,
	S,
	D,

	LCLICK,
	RCLICK,

	LSHIFT,
	LCTL,
	SPACE,
	LALT,



	COUNT
};


class Input {
public:
	Input();
	~Input();
	bool isDown(Key key) const;
	bool wasPressed(Key key) const;
	bool wasReleased(Key key) const;

	glm::vec2 mousePosition() const;
	glm::vec2 mouseDelta() const;

	void update();
private:

	struct InputState
	{

		std::array<bool, static_cast<size_t>(Key::COUNT)> keys{};
		std::array<bool, static_cast<size_t>(Key::COUNT)> pressed{};
		std::array<bool, static_cast<size_t>(Key::COUNT)> released{};

		glm::vec2 mousePos;
		glm::vec2 mouseDelta;
	};



	InputState state;
	void setKeyPressed(Key key);
	void setKeyReleased(Key key);
	void setMousePosition(glm::vec2 position);

	
	friend void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void GLFW_CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	friend void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);



};