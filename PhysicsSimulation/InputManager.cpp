#include "InputManager.h"

GLFWwindow* InputManager::window = nullptr;
std::vector<InputManager::MouseClick> InputManager::mouseClicks;

void InputManager::initialize(GLFWwindow* window)
{
	InputManager::window = window;
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

const std::vector<InputManager::MouseClick>& InputManager::getMouseClicks()
{
	return mouseClicks;
}

void InputManager::clearInputs()
{
	mouseClicks.clear();
}

bool InputManager::isMouseButtonPressed(int button)
{
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

bool InputManager::isKeyPressed(int key)
{
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	mouseClicks.push_back({ button, action, mods, xpos, ypos });
}

bool InputManager::MouseClick::isLeftButton() const
{
	return button == 0;
}

bool InputManager::MouseClick::isRightButton() const
{
	return button == 1;
}

bool InputManager::MouseClick::isMiddleButton() const
{
	return button == 2;
}

bool InputManager::MouseClick::isPressed() const
{
	return action == 1;
}

bool InputManager::MouseClick::isReleased() const
{
	return action == 0;
}
