#pragma once
#include <memory>
class Renderer;
struct GLFWwindow;
class Input;


class Window {
public:
	Window(int width, int height, const char* title);
	~Window();
	void close();
	bool shouldClose();
	void setupGLFWCallbacks(Input& input);
	void disableCursor(bool disabled);

	void pollWindow();

private:
	struct Impl;
	std::unique_ptr<Impl> context;

	friend GLFWwindow* getGLFWWindow(Window&);

};

