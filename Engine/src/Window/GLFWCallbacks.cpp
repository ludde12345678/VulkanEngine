#include "GLFWCallbacks.h"
#include "../EngineData.h"
#include <Renderer.h>
#include <Input.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void GLFW_FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height) {

}



void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (GLFWConfig::PRINT_DEBUG_INFO == true) {
        std::cout << "Key Event: " << GLFW_KEY_ESCAPE;
    }

    Input* data = static_cast<Input*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        data->setKeyPressed(Key::W);
    }
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::W);
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        data->setKeyPressed(Key::S);
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::S);
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        data->setKeyPressed(Key::A);
    }
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::A);
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        data->setKeyPressed(Key::D);
    }
    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::D);
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        data->setKeyPressed(Key::LSHIFT);
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::LSHIFT);
    }

    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        data->setKeyPressed(Key::LCTL);
    }
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::LCTL);
    }
    
}
void GLFW_CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    
    Input* data = static_cast<Input*>(glfwGetWindowUserPointer(window));
    data->setMousePosition(glm::vec2(xpos, ypos));
    /*
    Camera& cam = appCtx->frame.camera;

    if (appCtx->frame.inputState.mouselook) {

        if (cam.firstMouse)
        {
            cam.lastX = xpos;
            cam.lastY = ypos;
            cam.firstMouse = false;
            return;
        }

        float sensitivity = 0.001f;

        float dx = static_cast<float>(xpos - cam.lastX);
        float dy = static_cast<float>(cam.lastY - ypos); // invert Y

        cam.lastX = xpos;
        cam.lastY = ypos;

        cam.yaw += dx * sensitivity;
        cam.pitch += dy * sensitivity;

        cam.pitch = glm::clamp(
            cam.pitch,
            glm::radians(-89.0f),
            glm::radians(89.0f));

        cam.forward.x = cos(cam.yaw) * cos(cam.pitch);
        cam.forward.y = sin(cam.pitch);
        cam.forward.z = sin(cam.yaw) * cos(cam.pitch);

        cam.forward = glm::normalize(cam.forward);

    }
    */
}

void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

    Input* data = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        data->setKeyPressed(Key::LCLICK);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::LCLICK);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        data->setKeyPressed(Key::RCLICK);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        data->setKeyReleased(Key::RCLICK);
    }
    
}