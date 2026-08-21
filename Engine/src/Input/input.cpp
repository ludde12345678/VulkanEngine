#include <Input.h>


Input::Input()
{}

Input::~Input() = default;

bool Input::isDown(Key key) const
{
	size_t index = static_cast<size_t>(key);
	return state.keys[index];
}

bool Input::wasPressed(Key key) const
{
	size_t index = static_cast<size_t>(key);
	return state.pressed[index];
}

bool Input::wasReleased(Key key) const
{
	size_t index = static_cast<size_t>(key);
	return state.released[index];
}

glm::vec2 Input::mousePosition() const
{
	return state.mousePos;
}

glm::vec2 Input::mouseDelta() const
{
	return state.mouseDelta;
}

void Input::update()
{
	state.pressed.fill(false);
	state.released.fill(false);
	state.mouseDelta = glm::vec2(0.0f);
}


// Callback interface
void Input::setKeyPressed(Key key)
{
	size_t index = static_cast<size_t>(key);
	if (!state.keys[index])
		state.pressed[index] = true;

	state.keys[index] = true;
}

void Input::setKeyReleased(Key key)
{
	size_t index = static_cast<size_t>(key);
	state.keys[index] = false;
	state.released[index] = true;

}

void Input::setMousePosition(glm::vec2 position)
{
	state.mouseDelta = position - state.mousePos;
	state.mousePos = position;
}
