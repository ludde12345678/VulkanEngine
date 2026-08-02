#include "GLFWCallbacks.h"
#include "../App/appContext.h"


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

    AppContext* appCtx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        appCtx->frame.inputState.W = true;
    }
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        appCtx->frame.inputState.W = false;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        appCtx->frame.inputState.S = true;
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        appCtx->frame.inputState.S = false;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        appCtx->frame.inputState.A = true;
    }
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        appCtx->frame.inputState.A = false;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        appCtx->frame.inputState.D = true;
    }
    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        appCtx->frame.inputState.D = false;
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        appCtx->frame.inputState.LSHIFT = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        appCtx->frame.inputState.LSHIFT = false;
    }

    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        appCtx->frame.inputState.LCTR = true;
    }
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
        appCtx->frame.inputState.LCTR = false;
    }

}
void GLFW_CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    
    AppContext* appCtx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));

    Camera& cam = appCtx->frame.Camera;

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
        cam.forward.y = sin(cam.yaw) * cos(cam.pitch);
        cam.forward.z = sin(cam.pitch);

        cam.forward = glm::normalize(cam.forward);

    }
}

void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    AppContext* appCtx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        appCtx->frame.inputState.mouselook = true;
        appCtx->frame.Camera.firstMouse = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        appCtx->frame.inputState.mouselook = false;
    }
}