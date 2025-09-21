#include "LaurelEyeEngine/graphics/Window.h"

/* Private functions */

void Window::resizeCallbackWrapper(
    GLFWwindow* pWindow, int width, int height) {
    Window* window =
        static_cast<Window*>(glfwGetWindowUserPointer(pWindow));

    if ( window ) {
        window->width = width;
        window->height = height;

        if ( window->resizeCallback ) {
            window->resizeCallback(pWindow, width, height);
        }
    }
}

void Window::cursorPosCallbackWrapper(
  GLFWwindow* pWindow, double xpos, double ypos)
{
  Window* window =
    static_cast<Window*>(glfwGetWindowUserPointer(pWindow));

  if (window && window->cursorPosCallback)
  {
    window->cursorPosCallback(pWindow, xpos, ypos);
  }
}

void Window::mouseButtonCallbackWrapper(
  GLFWwindow* pWindow, int button, int action, int mods)
{
  Window* window =
    static_cast<Window*>(glfwGetWindowUserPointer(pWindow));

  if (window && window->mouseButtonCallback)
  {
    window->mouseButtonCallback(pWindow, button, action, mods);
  }
}


